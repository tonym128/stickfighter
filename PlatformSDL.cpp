#ifndef ARDUINO
#include "Platform.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <thread>

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static SDL_Texture* screenTexture = nullptr;
static uint32_t pixels[128 * 64];
static uint8_t currentButtons = 0;
static uint8_t previousButtons = 0;
static int frameRate = 60;
static auto lastFrameTime = std::chrono::steady_clock::now();

uint16_t Arduboy2::frameCount = 0;
bool Arduboy2::shouldRestart = false;

void Arduboy2::begin() {
    if (window) return;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }
    window = SDL_CreateWindow("StickFighter SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 128 * 4, 64 * 4, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 128, 64);
}

void Arduboy2::setFrameRate(int r) {
    frameRate = r;
}

bool Arduboy2::nextFrame() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrameTime).count();
    if (elapsed < (1000 / frameRate)) {
        return false;
    }
    lastFrameTime = now;
    frameCount++;
    return true;
}

void Arduboy2::pollButtons() {
    SDL_Event e;
    previousButtons = currentButtons;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            exit(0);
        }
    }

    const uint8_t* state = SDL_GetKeyboardState(NULL);
    currentButtons = 0;
    if (state[SDL_SCANCODE_LEFT]) currentButtons |= LEFT_BUTTON;
    if (state[SDL_SCANCODE_RIGHT]) currentButtons |= RIGHT_BUTTON;
    if (state[SDL_SCANCODE_UP]) currentButtons |= UP_BUTTON;
    if (state[SDL_SCANCODE_DOWN]) currentButtons |= DOWN_BUTTON;
    if (state[SDL_SCANCODE_Z]) currentButtons |= A_BUTTON;
    if (state[SDL_SCANCODE_X]) currentButtons |= B_BUTTON;
    if (state[SDL_SCANCODE_R]) shouldRestart = true;
}

void Arduboy2::clear() {
    memset(pixels, 0, sizeof(pixels));
}

void Arduboy2::display() {
    SDL_UpdateTexture(screenTexture, NULL, pixels, 128 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void Arduboy2::drawPixel(int16_t x, int16_t y, uint8_t color) {
    if (x < 0 || x >= 128 || y < 0 || y >= 64) return;
    pixels[y * 128 + x] = (color == WHITE) ? 0xFFFFFFFF : 0xFF000000;
}

void Arduboy2::drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color) {
    // Simple Bresenham's line algorithm
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;
    for (;;) {
        drawPixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void Arduboy2::drawRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t c) {
    drawFastHLine(x, y, w, c);
    drawFastHLine(x, y + h - 1, w, c);
    drawFastVLine(x, y, h, c);
    drawFastVLine(x + w - 1, y, h, c);
}

void Arduboy2::fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t c) {
    for (int i = 0; i < h; i++) drawFastHLine(x, y + i, w, c);
}

void Arduboy2::drawCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t c) {
    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int x = 0;
    int y = r;
    drawPixel(x0, y0 + r, c);
    drawPixel(x0, y0 - r, c);
    drawPixel(x0 + r, y0, c);
    drawPixel(x0 - r, y0, c);
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        drawPixel(x0 + x, y0 + y, c);
        drawPixel(x0 - x, y0 + y, c);
        drawPixel(x0 + x, y0 - y, c);
        drawPixel(x0 - x, y0 - y, c);
        drawPixel(x0 + y, y0 + x, c);
        drawPixel(x0 - y, y0 + x, c);
        drawPixel(x0 + y, y0 - x, c);
        drawPixel(x0 - y, y0 - x, c);
    }
}

void Arduboy2::fillCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t c) {
    for (int8_t y = -r; y <= r; y++) {
        for (int8_t x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                drawPixel(x0 + x, y0 + y, c);
            }
        }
    }
}

void Arduboy2::drawFastHLine(int16_t x, int16_t y, uint8_t w, uint8_t c) {
    for (int i = 0; i < w; i++) drawPixel(x + i, y, c);
}

void Arduboy2::drawFastVLine(int16_t x, int16_t y, uint8_t h, uint8_t c) {
    for (int i = 0; i < h; i++) drawPixel(x, y + i, c);
}

static int cursorX = 0, cursorY = 0;

// Simple 5x7 font
static const uint8_t font5x7[] = {
    0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
    0x00, 0x42, 0x7F, 0x40, 0x00, // 1
    0x42, 0x61, 0x51, 0x49, 0x46, // 2
    0x21, 0x41, 0x45, 0x4B, 0x31, // 3
    0x18, 0x14, 0x12, 0x7F, 0x10, // 4
    0x27, 0x45, 0x45, 0x45, 0x39, // 5
    0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
    0x01, 0x71, 0x09, 0x05, 0x03, // 7
    0x36, 0x49, 0x49, 0x49, 0x36, // 8
    0x06, 0x49, 0x49, 0x29, 0x1E, // 9
    0x7C, 0x12, 0x11, 0x12, 0x7C, // A
    0x7F, 0x49, 0x49, 0x49, 0x36, // B
    0x3E, 0x41, 0x41, 0x41, 0x22, // C
    0x7F, 0x41, 0x41, 0x22, 0x1C, // D
    0x7F, 0x49, 0x49, 0x49, 0x41, // E
    0x7F, 0x09, 0x09, 0x09, 0x01, // F
    0x3E, 0x41, 0x49, 0x49, 0x7A, // G
    0x7F, 0x08, 0x08, 0x08, 0x7F, // H
    0x00, 0x41, 0x7F, 0x41, 0x00, // I
    0x20, 0x40, 0x41, 0x3F, 0x01, // J
    0x7F, 0x08, 0x14, 0x22, 0x41, // K
    0x7F, 0x40, 0x40, 0x40, 0x40, // L
    0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
    0x7F, 0x04, 0x08, 0x10, 0x7F, // N
    0x3E, 0x41, 0x41, 0x41, 0x3E, // O
    0x7F, 0x09, 0x09, 0x09, 0x06, // P
    0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
    0x7F, 0x09, 0x19, 0x29, 0x46, // R
    0x46, 0x49, 0x49, 0x49, 0x31, // S
    0x01, 0x01, 0x7F, 0x01, 0x01, // T
    0x3F, 0x40, 0x40, 0x40, 0x3F, // U
    0x1F, 0x20, 0x40, 0x20, 0x1F, // V
    0x3F, 0x40, 0x38, 0x40, 0x3F, // W
    0x63, 0x14, 0x08, 0x14, 0x63, // X
    0x07, 0x08, 0x70, 0x08, 0x07, // Y
    0x61, 0x51, 0x49, 0x45, 0x43, // Z
    0x00, 0x00, 0x00, 0x00, 0x00, // Space
    0x08, 0x08, 0x3E, 0x08, 0x08, // +
    0x08, 0x08, 0x08, 0x08, 0x08, // -
    0x00, 0x33, 0x33, 0x00, 0x00, // :
    0x00, 0x60, 0x60, 0x00, 0x00, // .
    0x22, 0x14, 0x08, 0x14, 0x22, // *
    0x00, 0x41, 0x41, 0x00, 0x00, // = (placeholder)
    0x04, 0x02, 0x7F, 0x02, 0x04, // >
    0x10, 0x20, 0x7F, 0x20, 0x10, // <
    0x00, 0x00, 0x5F, 0x00, 0x00, // !
    0x02, 0x01, 0x51, 0x09, 0x06, // ?
    0x08, 0x08, 0x2A, 0x1C, 0x08, // {
    0x08, 0x1C, 0x2A, 0x08, 0x08  // }
};

void drawChar(int x, int y, char c, uint8_t color) {
    const uint8_t* glyph = nullptr;
    if (c >= '0' && c <= '9') glyph = &font5x7[(c - '0') * 5];
    else if (c >= 'A' && c <= 'Z') glyph = &font5x7[(c - 'A' + 10) * 5];
    else if (c >= 'a' && c <= 'z') glyph = &font5x7[(c - 'a' + 10) * 5];
    else if (c == ' ') glyph = &font5x7[36 * 5];
    else if (c == '+') glyph = &font5x7[37 * 5];
    else if (c == '-') glyph = &font5x7[38 * 5];
    else if (c == ':') glyph = &font5x7[39 * 5];
    else if (c == '.') glyph = &font5x7[40 * 5];
    else if (c == '*') glyph = &font5x7[41 * 5];
    else if (c == '=') glyph = &font5x7[42 * 5];
    else if (c == '>') glyph = &font5x7[43 * 5];
    else if (c == '<') glyph = &font5x7[44 * 5];
    else if (c == '!') glyph = &font5x7[45 * 5];
    else if (c == '?') glyph = &font5x7[46 * 5];
    else if (c == '{') glyph = &font5x7[47 * 5];
    else if (c == '}') glyph = &font5x7[48 * 5];
    else glyph = &font5x7[36 * 5]; // Default to space

    for (int i = 0; i < 5; i++) {
        uint8_t line = glyph[i];
        for (int j = 0; j < 7; j++) {
            if (line & (1 << j)) {
                Arduboy2().drawPixel(x + i, y + j, color);
            }
        }
    }
}

void Arduboy2::setCursor(int16_t x, int16_t y) {
    cursorX = x; cursorY = y;
}

void Arduboy2::print(const char* s) {
    while (*s) {
        if (*s == '\n') {
            cursorX = 0; // Or keep original indentation? Let's go to 0 for now.
            cursorY += 8;
        } else {
            drawChar(cursorX, cursorY, *s, WHITE);
            cursorX += 6;
        }
        s++;
    }
}

void Arduboy2::print(int32_t n) {
    char buf[12];
    snprintf(buf, sizeof(buf), "%d", n);
    print(buf);
}

void Arduboy2::print(uint32_t n) {
    char buf[12];
    snprintf(buf, sizeof(buf), "%u", n);
    print(buf);
}

bool Arduboy2::pressed(uint8_t b) {
    return currentButtons & b;
}

bool Arduboy2::justPressed(uint8_t b) {
    return (currentButtons & b) && !(previousButtons & b);
}

long random(long max) {
    return rand() % max;
}

long random(long min, long max) {
    if (max == min) return min;
    return min + (rand() % (max - min));
}

#endif

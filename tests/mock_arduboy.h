#ifndef MOCK_ARDUBOY_H
#define MOCK_ARDUBOY_H

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <iostream>

// Mock Arduino/Arduboy constants
#define PROGMEM
#define pgm_read_word(addr) (*(addr))
#define F(x) x

#define WHITE 1
#define BLACK 0

// Mock PROGMEM access
inline void memcpy_P(void* dest, const void* src, size_t n) { memcpy(dest, src, n); }

#define LEFT_BUTTON  (1 << 0)
#define RIGHT_BUTTON (1 << 1)
#define UP_BUTTON    (1 << 2)
#define DOWN_BUTTON  (1 << 3)
#define A_BUTTON     (1 << 4)
#define B_BUTTON     (1 << 5)

// Mock random
inline long random(long min, long max) { return min; }

// Mock Arduboy2 Class
class Arduboy2 {
public:
    uint8_t btns = 0;
    void begin() {}
    void setFrameRate(int r) {}
    bool nextFrame() { return true; }
    void pollButtons() {}
    void clear() {}
    void display() {}
    void setCursor(int x, int y) {}
    void print(const char* s) {}
    bool pressed(uint8_t b) { return btns & b; }
    bool justPressed(uint8_t b) { return btns & b; }
    void drawRect(int x, int y, int w, int h, int c) {}
    void fillRect(int x, int y, int w, int h, int c) {}
    void drawFastHLine(int x, int y, int w, int c) {}
    void drawFastVLine(int x, int y, int w, int c) {}
    void drawLine(int x1, int y1, int x2, int y2) {}
    uint8_t buttonsState() { return btns; }
};

#endif

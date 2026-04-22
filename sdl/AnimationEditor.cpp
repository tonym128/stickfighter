#include "AnimationEditor.h"
#include <fstream>
#include <iostream>

void AnimationEditor::setup() {
    gameDisplay.begin("StickFighter Preview", 128, 64, 4);
    guiDisplay.begin("StickFighter Controls", 160, 200, 2);
    
    for (uint8_t i = 0; i < 12; i++) {
        Pose p;
        memcpy_P(&p, &poses[i], sizeof(Pose));
        localPoses.push_back(p);
    }
    
    Engine::initSkeleton(skeleton, 0, TO_FP(0), false);
}

void AnimationEditor::save() {
    std::ofstream out("poses.h");
    if (!out.is_open()) return;
    out << "#ifndef POSES_H\n#define POSES_H\n\n#ifdef ARDUINO\n#include <Arduino.h>\n#else\n#include \"Platform.h\"\n#endif\n\nconst Pose poses[] PROGMEM = {\n";
    for (size_t i = 0; i < localPoses.size(); i++) {
        out << "    {{" << (int)localPoses[i].angles[0];
        for (int j = 1; j < 6; j++) out << ", " << (int)localPoses[i].angles[j];
        out << "}}";
        if (i < localPoses.size() - 1) out << ",";
        out << "   // " << i << ": " << animNames[i] << "\n";
    }
    out << "};\n\nconst char* const animNames[] = { ";
    for (size_t i = 0; i < 12; i++) { out << "\"" << animNames[i] << "\"" << (i < 11 ? ", " : ""); }
    out << " };\n\n#endif\n";
    out.close();
}

void AnimationEditor::updateSkeleton() {
    uint8_t pIdx = currentPoseIdx;
    if (isPlaying) {
        playbackFrame++;
        if (currentPoseIdx >= 1 && currentPoseIdx <= 4) pIdx = 1 + (playbackFrame / playbackSpeed) % 4;
    }
    Engine::updateSkeleton(skeleton, localPoses[pIdx], gameDisplay.frameCount, pIdx);
}

bool AnimationEditor::drawButton(Arduboy2 &display, int16_t x, int16_t y, int16_t w, int16_t h, const char* label, bool active) {
    bool hovered = (&display == &gameDisplay || &display == &guiDisplay) && // Mouse state is global for now
                  (display.mouseX >= x && display.mouseX < x + w && display.mouseY >= y && display.mouseY < y + h);
    bool clicked = hovered && display.mouseJustPressed;
    
    display.drawRect(x, y, w, h, WHITE);
    if (active) display.fillRect(x + 2, y + 2, w - 4, h - 4, WHITE);
    else if (hovered && display.mousePressed) display.drawRect(x + 1, y + 1, w - 2, h - 2, WHITE);
    
    display.setCursor(x + 4, y + (h - 7) / 2);
    display.print(label);
    return clicked;
}

void AnimationEditor::drawUI() {
    guiDisplay.clear();
    
    // Poses
    for (uint8_t i = 0; i < 12; i++) {
        if (drawButton(guiDisplay, 5, 5 + i * 12, 50, 10, animNames[i], currentPoseIdx == i)) currentPoseIdx = i;
    }
    
    // Bones
    const char* boneNames[] = {"TORSO", "HEAD", "R ARM", "L ARM", "R LEG", "L LEG"};
    for (uint8_t i = 0; i < 6; i++) {
        if (drawButton(guiDisplay, 65, 5 + i * 15, 45, 12, boneNames[i], currentBoneIdx == i)) currentBoneIdx = i;
    }
    
    // Angle
    guiDisplay.setCursor(65, 110);
    guiDisplay.print("ANGLE: "); guiDisplay.print((int)localPoses[currentPoseIdx].angles[currentBoneIdx]);
    if (drawButton(guiDisplay, 65, 120, 20, 15, "-")) localPoses[currentPoseIdx].angles[currentBoneIdx] -= 2;
    if (drawButton(guiDisplay, 90, 120, 20, 15, "+")) localPoses[currentPoseIdx].angles[currentBoneIdx] += 2;
    
    // Controls
    if (drawButton(guiDisplay, 120, 5, 35, 15, isPlaying ? "STOP" : "PLAY", isPlaying)) { isPlaying = !isPlaying; playbackFrame = 0; }
    if (drawButton(guiDisplay, 120, 25, 35, 15, "SAVE")) save();
    
    guiDisplay.display();
}

void AnimationEditor::loop() {
    if (!gameDisplay.nextFrame()) return;
    gameDisplay.pollButtons(); // Polls for all
    
    updateSkeleton();
    
    gameDisplay.clear();
    camera.x = skeleton.x; camera.y = skeleton.y - TO_FP(10);
    Engine::drawSkeleton(gameDisplay, skeleton, camera, 0);
    gameDisplay.display();
    
    drawUI();
}

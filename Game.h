#ifndef GAME_H
#define GAME_H

#include "Platform.h"
#include "game_data.h"

class Game {
public:
    void setup();
    void loop();

protected:
    void updateSkeleton(Skeleton &s);
    void drawSkeleton(Skeleton &s);
    void initSkeleton(Skeleton &s, uint8_t cIdx, int32_t x, bool faceLeft);
    void drawScaledLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    void drawScaledCircle(int32_t x, int32_t y, int8_t r);
    void drawFace(int16_t x, int16_t y, FaceData& f, bool flip, int16_t zoom);
    void drawBackground();
    void triggerHit(Skeleton &attacker, Skeleton &defender, bool isSuper = false);
    void updateAI();
    void resetRound();
    void updateFight();
    void drawFight();
    void drawTest2();
    void drawCharSelect();
    void drawLadder();
    void drawMenu();
    void drawOptions();
    void drawRoundOver();
    
    int16_t getSin(uint8_t angle);
    int16_t getCos(uint8_t angle);

    Arduboy2 arduboy;
    GameState currentState = STATE_TITLE;
    struct { int32_t x, y; int16_t zoom; } camera = { TO_FP(64), TO_FP(32), 100 };
    Skeleton player, opponent;
    uint8_t shakeTimer = 0, freezeTimer = 0;
    uint8_t selectedChar = 0, ladderStage = 0;
    uint8_t playerWins = 0, opponentWins = 0;
    uint16_t roundOverTimer = 0;
    uint8_t menuIdx = 0;
    bool audioOn = true, sfxOn = true, musicOn = true;
    uint8_t testAnimIdx = 0;
    uint8_t editMode = 0; 
    bool isAutoplay = false;
    uint8_t editBoneIdx = 0;
    Pose editablePose = {{194, 190, 35, 155, 75, 115}};
};

#endif

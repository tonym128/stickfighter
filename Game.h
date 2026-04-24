#ifndef GAME_H
#define GAME_H

#include "Platform.h"
#include "game_data.h"
#include "Engine.h"
#include "logo.h"

class Game {
public:
    void setup();
    void loop();

protected:
    void drawBackground();
    void triggerHit(Skeleton &attacker, Skeleton &defender, bool isSuper = false);
    void updateAI();
    void resetRound();
    void updateFight();
    void drawFight();
    void drawCharSelect();
    void drawLadder();
    void drawMenu();
    void drawRoundOver();
    
    void updateInputBuffer();
    bool checkCombo(const uint8_t* sequence, uint8_t length);
    bool handleSpecials();
    void updateProjectiles();
    void drawProjectiles();
    
    Arduboy2 arduboy;
    GameState currentState = STATE_TITLE;
    Camera camera = { TO_FP(64), TO_FP(32), 100 };
    Skeleton player, opponent;
    Skeleton leftFighter, rightFighter;
    InputBuffer playerBuffer;
    Projectile projectiles[MAX_PROJECTILES];
    
    uint8_t shakeTimer = 0, freezeTimer = 0;
    uint8_t selectedChar = 0, ladderStage = 0;
    uint8_t playerWins = 0, opponentWins = 0;
    uint16_t roundOverTimer = 0;
    uint8_t menuIdx = 0;
};

#endif

#include "Game.h"

void Game::drawBackground() {
    Engine::drawScaledLine(arduboy, TO_FP(-1000), GROUND_Y, TO_FP(1000), GROUND_Y, camera, shakeTimer);
    uint8_t stage = ladderStage % 6;
    switch(stage) {
        case 0: for(int16_t x=-400;x<=400;x+=200){Engine::drawScaledLine(arduboy, TO_FP(x),GROUND_Y,TO_FP(x),GROUND_Y-TO_FP(30), camera, shakeTimer);Engine::drawScaledLine(arduboy, TO_FP(x),GROUND_Y-TO_FP(30),TO_FP(x-15),GROUND_Y-TO_FP(25), camera, shakeTimer);Engine::drawScaledLine(arduboy, TO_FP(x),GROUND_Y-TO_FP(30),TO_FP(x+15),GROUND_Y-TO_FP(25), camera, shakeTimer);} break;
        case 1: Engine::drawScaledLine(arduboy, TO_FP(-500),GROUND_Y-TO_FP(5),TO_FP(-200),GROUND_Y-TO_FP(15), camera, shakeTimer);Engine::drawScaledLine(arduboy, TO_FP(-200),GROUND_Y-TO_FP(15),TO_FP(100),GROUND_Y-TO_FP(5), camera, shakeTimer);Engine::drawScaledLine(arduboy, TO_FP(100),GROUND_Y-TO_FP(5),TO_FP(400),GROUND_Y-TO_FP(20), camera, shakeTimer); break;
        case 2: for(int16_t x=-350;x<=350;x+=150){Engine::drawScaledLine(arduboy, TO_FP(x),GROUND_Y,TO_FP(x+20),GROUND_Y-TO_FP(15), camera, shakeTimer);Engine::drawScaledLine(arduboy, TO_FP(x+20),GROUND_Y-TO_FP(15),TO_FP(x+40),GROUND_Y, camera, shakeTimer);} break;
        case 3: Engine::drawScaledCircle(arduboy, TO_FP(0),GROUND_Y,20, camera, shakeTimer);for(int i=0;i<8;i++){int16_t dx=(Engine::getCos(i*32)*30)>>8;int16_t dy=(Engine::getSin(i*32)*30)>>8;Engine::drawScaledLine(arduboy, TO_FP(0),GROUND_Y,TO_FP(dx),GROUND_Y+TO_FP(dy), camera, shakeTimer);} break;
        case 4: Engine::drawScaledCircle(arduboy, TO_FP(-80),TO_FP(-50),10, camera, shakeTimer);Engine::drawScaledCircle(arduboy, TO_FP(-76),TO_FP(-50),8, camera, shakeTimer); break;
        case 5: static uint16_t rOff=0;rOff+=4;for(int i=0;i<15;i++){int16_t rx=(i*71)%400-200;int16_t ry=(i*37+rOff)%150-50;Engine::drawScaledLine(arduboy, TO_FP(rx),TO_FP(ry),TO_FP(rx+2),TO_FP(ry+6), camera, shakeTimer);} break;
    }
}

void Game::triggerHit(Skeleton &attacker, Skeleton &defender, bool isSuper) {
    shakeTimer = isSuper ? 20 : 8; freezeTimer = isSuper ? 15 : 5;
    uint8_t fwd = defender.facingLeft ? LEFT_BUTTON : RIGHT_BUTTON;
    if (arduboy.pressed(fwd) && defender.stateTimer < 8) { defender.state = CS_IDLE; defender.special += 25; attacker.state = CS_PARRY_STUN; attacker.stateTimer = 40; return; }
    int8_t dmg = isSuper ? 30 : 10;
    int16_t kb = isSuper ? TO_FP(5) : TO_FP(3);
    if (attacker.x < defender.x) { defender.vx = kb; attacker.vx = -kb/2; } else { defender.vx = -kb; attacker.vx = kb/2; }
    if (defender.state == CS_BLOCK) { defender.health -= (dmg / 5); defender.vx /= 2; defender.special += 2; attacker.special += 5; } else { defender.health -= dmg; defender.state = CS_HITSTUN; defender.stateTimer = isSuper ? 30 : 15; defender.special += 5; attacker.special += 10; }
    if (defender.health < 0) defender.health = 0; if (defender.special > 100) defender.special = 100; if (attacker.special > 100) attacker.special = 100;
}

void Game::updateAI() {
    if (opponent.stateTimer > 0) return; if (opponent.aiTimer > 0) { opponent.aiTimer--; return; }
    int32_t dist = labs(FROM_FP(player.x - opponent.x)); uint8_t aggression = 20 + (ladderStage * 8); 
    switch (opponent.aiState) {
        case AI_IDLE: opponent.aiState = (dist > 45) ? AI_APPROACH : AI_WAIT; opponent.aiTimer = random(10, 30 - ladderStage * 2); break;
        case AI_APPROACH: opponent.vx += opponent.facingLeft ? -ACCEL : ACCEL; if (dist < 40) { opponent.aiState = AI_ATTACKING; } break;
        case AI_WAIT: opponent.vx = 0; if (random(0, 100) < aggression) opponent.aiState = AI_ATTACKING; else if (dist < 30) opponent.aiState = AI_RETREAT; opponent.aiTimer = random(20, 40); break;
        case AI_RETREAT: opponent.vx += opponent.facingLeft ? ACCEL : -ACCEL; if (dist > 60 || random(0, 10) == 0) opponent.aiState = AI_IDLE; break;
        case AI_ATTACKING:
            opponent.vx = 0;
            uint8_t r = random(0, 10);
            if (r == 0) { // Special move
                opponent.state = CS_SPECIAL_STARTUP; opponent.stateTimer = 15;
            } else if (r < 3) {
                opponent.state = CS_PUNCH_STARTUP; opponent.stateTimer = 8;
            } else if (r < 6) {
                opponent.state = CS_KICK_STARTUP; opponent.stateTimer = 10;
            } else if (r < 8) {
                opponent.state = CS_DUCK_PUNCH_STARTUP; opponent.stateTimer = 8;
            } else {
                opponent.state = CS_DUCK_KICK_STARTUP; opponent.stateTimer = 10;
            }
            opponent.aiState = AI_IDLE; opponent.aiTimer = random(30, 60);
            break;
    }
}

void Game::resetRound() { 
    Engine::initSkeleton(player, player.charIdx, TO_FP(30), false); 
    Engine::initSkeleton(opponent, opponent.charIdx, TO_FP(100), true); 
    for(int i=0; i<MAX_PROJECTILES; i++) projectiles[i].active = false;
}

void Game::updateFight() {
    if (freezeTimer > 0) { freezeTimer--; return; }
    if (shakeTimer > 0) shakeTimer--; 
    
    updateInputBuffer();
    updateProjectiles();
    
    if (player.stateTimer > 0) player.stateTimer--;
    if (player.state <= CS_DUCK) {
        handleSpecials();
        uint8_t back = player.facingLeft?RIGHT_BUTTON:LEFT_BUTTON, fwd = player.facingLeft?LEFT_BUTTON:RIGHT_BUTTON;
        if (arduboy.pressed(back)) { 
            int32_t dist = labs(FROM_FP(player.x - opponent.x));
            bool oppAttacking = (opponent.state == CS_PUNCH_STARTUP || opponent.state == CS_PUNCH_ACTIVE || 
                                 opponent.state == CS_KICK_STARTUP || opponent.state == CS_KICK_ACTIVE || 
                                 opponent.state == CS_SUPER_STARTUP);
            if (oppAttacking && dist < 50) {
                player.state = CS_BLOCK; player.vx = 0; 
            } else {
                player.vx += player.facingLeft ? ACCEL : -ACCEL;
                player.state = CS_WALK;
            }
        } else if (arduboy.pressed(DOWN_BUTTON)) {
            player.state = CS_DUCK; player.vx = 0;
            if (arduboy.justPressed(A_BUTTON)) { player.state = CS_DUCK_PUNCH_STARTUP; player.stateTimer = 8; }
            else if (arduboy.justPressed(B_BUTTON)) { player.state = CS_DUCK_KICK_STARTUP; player.stateTimer = 10; }
        }
        else if (arduboy.pressed(fwd)) { player.vx += player.facingLeft ? -ACCEL : ACCEL; player.state = CS_WALK; }
        else { player.state = CS_IDLE; if (player.vx > 0) player.vx -= FRICTION; else if (player.vx < 0) player.vx += FRICTION; if (abs(player.vx) < FRICTION) player.vx = 0; }
        if (player.vx > player.walkSpeed) player.vx = player.walkSpeed; if (player.vx < -player.walkSpeed) player.vx = -player.walkSpeed;
        if (arduboy.pressed(UP_BUTTON) && !player.isJumping) { player.vy = JUMP_IMPULSE; player.isJumping = true; }
        if (arduboy.pressed(A_BUTTON) && arduboy.pressed(B_BUTTON) && player.special >= 100) { player.state = CS_SUPER_STARTUP; player.stateTimer = 40; player.special = 0; freezeTimer = 40; } 
        else if (arduboy.justPressed(A_BUTTON) && player.state != CS_DUCK_PUNCH_STARTUP) { player.state = CS_PUNCH_STARTUP; player.stateTimer = 8; } 
        else if (arduboy.justPressed(B_BUTTON) && player.state != CS_DUCK_KICK_STARTUP) { player.state = CS_KICK_STARTUP; player.stateTimer = 10; }
    } else if (player.stateTimer == 0) {
        if (player.state == CS_SUPER_STARTUP) { player.state = CS_PUNCH_ACTIVE; player.stateTimer = 20; }
        else if (player.state == CS_PUNCH_STARTUP) { player.state = CS_PUNCH_ACTIVE; player.stateTimer = 12; }
        else if (player.state == CS_PUNCH_ACTIVE) { player.state = CS_PUNCH_RECOVERY; player.stateTimer = 20; }
        else if (player.state == CS_KICK_STARTUP) { player.state = CS_KICK_ACTIVE; player.stateTimer = 15; }
        else if (player.state == CS_KICK_ACTIVE) { player.state = CS_KICK_RECOVERY; player.stateTimer = 25; }
        else if (player.state == CS_DUCK_PUNCH_STARTUP) { player.state = CS_DUCK_PUNCH_ACTIVE; player.stateTimer = 12; }
        else if (player.state == CS_DUCK_PUNCH_ACTIVE) { player.state = CS_DUCK_PUNCH_RECOVERY; player.stateTimer = 15; }
        else if (player.state == CS_DUCK_KICK_STARTUP) { player.state = CS_DUCK_KICK_ACTIVE; player.stateTimer = 18; }
        else if (player.state == CS_DUCK_KICK_ACTIVE) { player.state = CS_DUCK_KICK_RECOVERY; player.stateTimer = 20; }
        else if (player.state == CS_SPECIAL_STARTUP) { 
            player.state = CS_SPECIAL_ACTIVE; player.stateTimer = 15; 
            if (player.charIdx == 0) { // ZENITH - Spawn Fireball
                for(int i=0; i<MAX_PROJECTILES; i++) if(!projectiles[i].active) {
                    projectiles[i].active = true; projectiles[i].ownerIsPlayer = true;
                    projectiles[i].x = player.x; projectiles[i].y = player.y - TO_FP(12);
                    projectiles[i].vx = player.facingLeft ? TO_FP(-3) : TO_FP(3);
                    break;
                }
            }
        }
        else if (player.state == CS_SPECIAL_ACTIVE) { player.state = CS_SPECIAL_RECOVERY; player.stateTimer = 15; }
        else player.state = CS_IDLE;
    }
    if (opponent.stateTimer > 0) opponent.stateTimer--;
    if (opponent.state <= CS_DUCK) { updateAI(); if (opponent.vx > 0) opponent.vx -= FRICTION/2; else if (opponent.vx < 0) opponent.vx += FRICTION/2; } 
    else if (opponent.stateTimer == 0) { 
        if (opponent.state == CS_PUNCH_STARTUP) { opponent.state = CS_PUNCH_ACTIVE; opponent.stateTimer = 12; } 
        else if (opponent.state == CS_PUNCH_ACTIVE) { opponent.state = CS_PUNCH_RECOVERY; opponent.stateTimer = 20; } 
        else if (opponent.state == CS_KICK_STARTUP) { opponent.state = CS_KICK_ACTIVE; opponent.stateTimer = 15; } 
        else if (opponent.state == CS_KICK_ACTIVE) { opponent.state = CS_KICK_RECOVERY; opponent.stateTimer = 25; } 
        else if (opponent.state == CS_DUCK_PUNCH_STARTUP) { opponent.state = CS_DUCK_PUNCH_ACTIVE; opponent.stateTimer = 12; }
        else if (opponent.state == CS_DUCK_PUNCH_ACTIVE) { opponent.state = CS_DUCK_PUNCH_RECOVERY; opponent.stateTimer = 15; }
        else if (opponent.state == CS_DUCK_KICK_STARTUP) { opponent.state = CS_DUCK_KICK_ACTIVE; opponent.stateTimer = 18; }
        else if (opponent.state == CS_DUCK_KICK_ACTIVE) { opponent.state = CS_DUCK_KICK_RECOVERY; opponent.stateTimer = 20; }
        else if (opponent.state == CS_SPECIAL_STARTUP) { 
            opponent.state = CS_SPECIAL_ACTIVE; opponent.stateTimer = 15; 
            if (opponent.charIdx == 0) { // ZENITH - Spawn Fireball
                for(int i=0; i<MAX_PROJECTILES; i++) if(!projectiles[i].active) {
                    projectiles[i].active = true; projectiles[i].ownerIsPlayer = false;
                    projectiles[i].x = opponent.x; projectiles[i].y = opponent.y - TO_FP(12);
                    projectiles[i].vx = opponent.facingLeft ? TO_FP(-3) : TO_FP(3);
                    break;
                }
            }
        }
        else if (opponent.state == CS_SPECIAL_ACTIVE) { opponent.state = CS_SPECIAL_RECOVERY; opponent.stateTimer = 15; }
        else opponent.state = CS_IDLE; 
    }
    player.facingLeft = (player.x > opponent.x); opponent.facingLeft = !player.facingLeft;
    player.vy += GRAVITY; player.x += player.vx; player.y += player.vy; if (player.y >= GROUND_Y) { player.y = GROUND_Y; player.vy = 0; player.isJumping = false; }
    opponent.vy += GRAVITY; opponent.x += opponent.vx; opponent.y += opponent.vy; if (opponent.y >= GROUND_Y) { opponent.y = GROUND_Y; opponent.vy = 0; opponent.isJumping = false; }
    if (player.x < TO_FP(-1000)) player.x = TO_FP(-1000); if (player.x > TO_FP(1000)) player.x = TO_FP(1000);
    if (opponent.x < TO_FP(-1000)) opponent.x = TO_FP(-1000); if (opponent.x > TO_FP(1000)) opponent.x = TO_FP(1000);

    uint8_t pIdx = 0;
    if (player.state == CS_PUNCH_ACTIVE) pIdx = 6;
    else if (player.state == CS_KICK_ACTIVE) pIdx = 7;
    else if (player.state == CS_DUCK_PUNCH_ACTIVE) pIdx = 10;
    else if (player.state == CS_DUCK_KICK_ACTIVE) pIdx = 11;
    else if (player.state == CS_SPECIAL_ACTIVE) pIdx = (player.charIdx == 0) ? 6 : 7; // Fireball: Punch, others Kick for now
    else if (player.state == CS_HITSTUN) pIdx = 9;
    else if (player.state == CS_BLOCK) pIdx = 5;
    else if (IS_DUCKING(player.state)) pIdx = 8;
    else if (player.state == CS_WALK) pIdx = (arduboy.frameCount / 8) % 4 + 1;
    else if (player.isJumping) pIdx = 7;
    Pose pTarget; memcpy_P(&pTarget, &poses[pIdx], sizeof(Pose)); Engine::updateSkeleton(player, pTarget, arduboy.frameCount, pIdx);

    uint8_t oIdx = 0;
    if (opponent.state == CS_PUNCH_ACTIVE) oIdx = 6;
    else if (opponent.state == CS_KICK_ACTIVE) oIdx = 7;
    else if (opponent.state == CS_DUCK_PUNCH_ACTIVE) oIdx = 10;
    else if (opponent.state == CS_DUCK_KICK_ACTIVE) oIdx = 11;
    else if (opponent.state == CS_SPECIAL_ACTIVE) oIdx = (opponent.charIdx == 0) ? 6 : 7;
    else if (opponent.state == CS_HITSTUN) oIdx = 9;
    else if (opponent.state == CS_BLOCK) oIdx = 5;
    else if (IS_DUCKING(opponent.state)) oIdx = 8;
    else if (opponent.state == CS_WALK) oIdx = (arduboy.frameCount / 8) % 4 + 1;
    else if (opponent.isJumping) oIdx = 7;
    Pose oTarget; memcpy_P(&oTarget, &poses[oIdx], sizeof(Pose)); Engine::updateSkeleton(opponent, oTarget, arduboy.frameCount, oIdx);

    if (player.state == CS_PUNCH_ACTIVE || player.state == CS_KICK_ACTIVE || player.state == CS_DUCK_PUNCH_ACTIVE || player.state == CS_DUCK_KICK_ACTIVE) {
        uint8_t hitBone = (player.state == CS_PUNCH_ACTIVE || player.state == CS_DUCK_PUNCH_ACTIVE) ? 3 : 5;
        for(uint8_t j=0; j<MAX_BONES; j++) if (opponent.bones[j].isHurtbox) {
            int32_t dx = FROM_FP(player.worldX[hitBone] - opponent.worldX[j]), dy = FROM_FP(player.worldY[hitBone] - opponent.worldY[j]);
            if (dx*dx + dy*dy < 25) { 
                triggerHit(player, opponent, (player.stateTimer > 15)); break; 
            }
        }
    }
    if (opponent.state == CS_PUNCH_ACTIVE || opponent.state == CS_KICK_ACTIVE || opponent.state == CS_DUCK_PUNCH_ACTIVE || opponent.state == CS_DUCK_KICK_ACTIVE) {
        uint8_t hitBone = (opponent.state == CS_PUNCH_ACTIVE || opponent.state == CS_DUCK_PUNCH_ACTIVE) ? 3 : 5;
        for(uint8_t j=0; j<MAX_BONES; j++) if (player.bones[j].isHurtbox) {
            int32_t dx = FROM_FP(opponent.worldX[hitBone] - player.worldX[j]), dy = FROM_FP(opponent.worldY[hitBone] - player.worldY[j]);
            if (dx*dx + dy*dy < 25) { triggerHit(opponent, player); break; }
        }
    }
    int32_t centerX = (player.x + opponent.x) / 2, centerY = (player.y + opponent.y) / 2 - TO_FP(10); camera.x = centerX; camera.y = centerY;
    int32_t dist = labs(FROM_FP(player.x - opponent.x)); if (dist < 25) dist = 25; camera.zoom = 4000 / dist; if (camera.zoom > 160) camera.zoom = 160; if (camera.zoom < 60) camera.zoom = 60;
    if (player.health == 0 || opponent.health == 0) { if (player.health == 0) opponentWins++; else playerWins++; currentState = STATE_ROUND_OVER; roundOverTimer = 120; }
}

void Game::drawFight() { 
    drawBackground(); 
    int32_t z = camera.zoom;
    int16_t screenX = (int16_t)((((player.x - camera.x) * z) / 100) >> FP_SHIFT) + 64; 
    int16_t screenGroundY = (int16_t)((((GROUND_Y - camera.y) * z) / 100) >> FP_SHIFT) + 32;
    arduboy.fillRect(screenX - 10, screenGroundY + 1, 20, 1, WHITE); 
    Engine::drawSkeleton(arduboy, player, camera, shakeTimer); 
    Engine::drawSkeleton(arduboy, opponent, camera, shakeTimer); 
    drawProjectiles();
    arduboy.drawRect(2, 2, 52, 5, WHITE); 
    arduboy.fillRect(3, 3, player.health/2, 3, WHITE); 
    arduboy.drawRect(74, 2, 52, 5, WHITE); 
    arduboy.fillRect(75 + (50 - opponent.health/2), 3, opponent.health/2, 3, WHITE); 
    if (playerWins >= 1) arduboy.fillCircle(2, 10, 2, WHITE); 
    if (playerWins >= 2) arduboy.fillCircle(8, 10, 2, WHITE); 
    if (opponentWins >= 1) arduboy.fillCircle(125, 10, 2, WHITE); 
    if (opponentWins >= 2) arduboy.fillCircle(119, 10, 2, WHITE); 
}

void Game::drawTest2() {
    arduboy.setCursor(2, 2); arduboy.print(F("ED: ")); CharacterData cd; memcpy_P(&cd, &roster[player.charIdx], sizeof(CharacterData)); arduboy.print(cd.name);
    arduboy.setCursor(2, 10); arduboy.print(F("AN: ")); arduboy.print(animNames[testAnimIdx]);
    arduboy.setCursor(2, 18); arduboy.print(F("BN: ")); arduboy.print(editBoneIdx);
    arduboy.setCursor(2, 26); arduboy.print(F("AG: ")); arduboy.print(editablePose.angles[editBoneIdx]);
    arduboy.setCursor(2, 34); arduboy.print(isAutoplay ? F("AUTO: ON") : F("AUTO: OFF"));
    Engine::updateSkeleton(player, editablePose, arduboy.frameCount, 255); Engine::drawSkeleton(arduboy, player, camera, 0);
    arduboy.setCursor(0, 56); arduboy.print(F("{{")); for(int i=0;i<6;i++){arduboy.print(editablePose.angles[i]); if(i<5)arduboy.print(F(","));} arduboy.print(F("}}"));
    if (arduboy.justPressed(B_BUTTON)) currentState = STATE_TITLE;
    if (arduboy.justPressed(UP_BUTTON) && editMode > 0) editMode--;
    if (arduboy.justPressed(DOWN_BUTTON) && editMode < 4) editMode++;
    if (editMode == 0) { if (arduboy.justPressed(LEFT_BUTTON) && player.charIdx > 0) player.charIdx--; if (arduboy.justPressed(RIGHT_BUTTON) && player.charIdx < 9) player.charIdx++; }
    else if (editMode == 1) { if (arduboy.justPressed(LEFT_BUTTON) && testAnimIdx > 0) { testAnimIdx--; memcpy_P(&editablePose, &poses[testAnimIdx], sizeof(Pose)); } if (arduboy.justPressed(RIGHT_BUTTON) && testAnimIdx < 11) { testAnimIdx++; memcpy_P(&editablePose, &poses[testAnimIdx], sizeof(Pose)); } }
    else if (editMode == 2) { if (arduboy.justPressed(LEFT_BUTTON) && editBoneIdx > 0) editBoneIdx--; if (arduboy.justPressed(RIGHT_BUTTON) && editBoneIdx < 5) editBoneIdx++; }
    else if (editMode == 3) { if (arduboy.pressed(LEFT_BUTTON)) editablePose.angles[editBoneIdx]-=2; if (arduboy.pressed(RIGHT_BUTTON)) editablePose.angles[editBoneIdx]+=2; }
    else if (editMode == 4) { if (arduboy.justPressed(A_BUTTON)) isAutoplay = !isAutoplay; }
    arduboy.drawFastVLine(70, 2 + editMode*8, 6);
}

void Game::drawCharSelect() {
    arduboy.setCursor(30, 2); arduboy.print(F("SELECT HERO"));
    for(uint8_t i=0; i<10; i++) { uint8_t x = 5 + (i%5)*24, y = 12 + (i/5)*20; arduboy.drawRect(x, y, 20, 18, (selectedChar == i) ? WHITE : BLACK); CharacterData d; memcpy_P(&d, &roster[i], sizeof(CharacterData)); Engine::drawFace(arduboy, x+10, y+8, d.face, false, 150); if (selectedChar == i) { arduboy.setCursor(40, 54); arduboy.print(d.name); } }
    if (arduboy.justPressed(LEFT_BUTTON) && selectedChar > 0) selectedChar--; if (arduboy.justPressed(RIGHT_BUTTON) && selectedChar < 9) selectedChar++;
    if (arduboy.justPressed(A_BUTTON)) { ladderStage = 0; playerWins = 0; opponentWins = 0; currentState = STATE_LADDER; }
}

void Game::drawLadder() {
    arduboy.setCursor(45, 5); arduboy.print(F("LADDER"));
    int8_t start = (int8_t)ladderStage - 2;
    for (int8_t i = 0; i < 4; i++) {
        int8_t idx = start + i;
        if (idx < 0 || idx >= 10) continue;
        uint8_t y = 18 + (i * 11);
        arduboy.setCursor(35, y);
        if (idx == ladderStage) arduboy.print(F("> "));
        else arduboy.print(F("  "));
        CharacterData cd;
        memcpy_P(&cd, &roster[idx], sizeof(CharacterData));
        arduboy.print(cd.name);
        if (idx < ladderStage) {
            uint8_t len = 0; while(cd.name[len] && len < 8) len++;
            arduboy.drawFastHLine(47, y + 3, len * 6, WHITE);
        }
    }
    if (ladderStage >= 10) currentState = STATE_RESULTS;
    else if (arduboy.justPressed(A_BUTTON)) { Engine::initSkeleton(player, selectedChar, TO_FP(30), false); Engine::initSkeleton(opponent, ladderStage, TO_FP(100), true); currentState = STATE_FIGHT; }
}

void Game::drawMenu() {
    arduboy.setCursor(25, 5); arduboy.print(F("STICK FIGHTER"));
    const char* options[] = {"START", "OPTIONS", "EDITOR"};
    for(uint8_t i=0; i<3; i++) { arduboy.setCursor(40, 20 + (i*12)); if (menuIdx == i) arduboy.print(F("> ")); arduboy.print(options[i]); }
    if (arduboy.justPressed(UP_BUTTON) && menuIdx > 0) menuIdx--; if (arduboy.justPressed(DOWN_BUTTON) && menuIdx < 2) menuIdx++;
    if (arduboy.justPressed(A_BUTTON)) { if (menuIdx == 0) currentState = STATE_CHAR_SELECT; else if (menuIdx == 1) currentState = STATE_OPTIONS; else if (menuIdx == 2) { Engine::initSkeleton(player, 0, TO_FP(80), false); camera.zoom = 150; camera.x = player.x; camera.y = player.y - TO_FP(10); currentState = STATE_TEST2; } }
}

void Game::drawOptions() {
    arduboy.setCursor(35, 5); arduboy.print(F("OPTIONS"));
    const char* opts[] = {"AUDIO", "SFX", "MUSIC"}; bool vals[] = {audioOn, sfxOn, musicOn};
    for(uint8_t i=0; i<3; i++) { arduboy.setCursor(20, 20 + (i*12)); if (menuIdx == i) arduboy.print(F("> ")); arduboy.print(opts[i]); arduboy.setCursor(80, 20 + (i*12)); arduboy.print(vals[i] ? F("ON") : F("OFF")); }
    if (arduboy.justPressed(UP_BUTTON) && menuIdx > 0) menuIdx--; if (arduboy.justPressed(DOWN_BUTTON) && menuIdx < 2) menuIdx++;
    if (arduboy.justPressed(LEFT_BUTTON) || arduboy.justPressed(RIGHT_BUTTON) || arduboy.justPressed(A_BUTTON)) { if (menuIdx == 0) audioOn = !audioOn; else if (menuIdx == 1) sfxOn = !sfxOn; else if (menuIdx == 2) musicOn = !musicOn; }
    if (arduboy.justPressed(B_BUTTON)) { menuIdx = 1; currentState = STATE_TITLE; }
}

void Game::drawRoundOver() {
    drawFight();
    arduboy.setCursor(45, 25);
    if (player.health == 0) arduboy.print(F("K.O."));
    else arduboy.print(F("WIN"));
    if (roundOverTimer > 0) roundOverTimer--;
    else {
        if (playerWins >= 2) {
            ladderStage++;
            playerWins = 0; opponentWins = 0;
            if (ladderStage >= 10) currentState = STATE_RESULTS;
            else currentState = STATE_LADDER;
        } else if (opponentWins >= 2) {
            currentState = STATE_RESULTS;
        } else {
            resetRound();
            currentState = STATE_FIGHT;
        }
    }
}

void Game::updateInputBuffer() {
    playerBuffer.buttons[playerBuffer.head] = arduboy.buttonsState();
    playerBuffer.head = (playerBuffer.head + 1) % INPUT_BUFFER_SIZE;
}

bool Game::checkCombo(const uint8_t* sequence, uint8_t length) {
    uint8_t seqIdx = length - 1;
    uint8_t bufIdx = (playerBuffer.head + INPUT_BUFFER_SIZE - 1) % INPUT_BUFFER_SIZE;
    
    uint8_t lastMatchedButtons = 0xFF; // Start with invalid
    uint16_t framesSearched = 0;
    const uint16_t MAX_COMBO_WINDOW = 45; // 0.75 seconds to complete motion

    while (framesSearched < INPUT_BUFFER_SIZE && framesSearched < MAX_COMBO_WINDOW) {
        uint8_t buttons = playerBuffer.buttons[bufIdx];
        uint8_t target = sequence[seqIdx];
        
        // Handle directions relative to facing
        if (target & RIGHT_BUTTON) {
            target &= ~RIGHT_BUTTON;
            target |= player.facingLeft ? LEFT_BUTTON : RIGHT_BUTTON;
        } else if (target & LEFT_BUTTON) {
            target &= ~LEFT_BUTTON;
            target |= player.facingLeft ? RIGHT_BUTTON : LEFT_BUTTON;
        }

        // Leniency: If target is a diagonal (e.g. DOWN|RIGHT), 
        // also accept just DOWN or just RIGHT if we are in the middle of a motion.
        // But for the FINAL button (usually A or B), require exact match.
        bool match = false;
        if (seqIdx == length - 1) {
            match = (buttons & target) == target;
        } else {
            // For motion parts, be more loose.
            if ((buttons & target) == target) match = true;
            // If it's a diagonal target, accept components
            else if ((target == (DOWN_BUTTON | RIGHT_BUTTON) || target == (DOWN_BUTTON | LEFT_BUTTON))) {
                if ((buttons & DOWN_BUTTON) || (buttons & (target & (LEFT_BUTTON | RIGHT_BUTTON)))) match = true;
            }
        }

        if (match && buttons != lastMatchedButtons) {
            if (seqIdx == 0) return true;
            seqIdx--;
            lastMatchedButtons = buttons;
        }
        
        bufIdx = (bufIdx + INPUT_BUFFER_SIZE - 1) % INPUT_BUFFER_SIZE;
        framesSearched++;
    }
    return false;
}

void Game::handleSpecials() {
    if (player.state > CS_DUCK) return;
    
    // Define combos
    static const uint8_t fireballSeq[] = {DOWN_BUTTON, RIGHT_BUTTON, A_BUTTON};
    static const uint8_t uppercutSeq[] = {RIGHT_BUTTON, DOWN_BUTTON, RIGHT_BUTTON, A_BUTTON};
    static const uint8_t hurricaneSeq[] = {DOWN_BUTTON, LEFT_BUTTON, B_BUTTON};
    static const uint8_t sumoSeq[]     = {LEFT_BUTTON, DOWN_BUTTON, RIGHT_BUTTON, A_BUTTON};
    static const uint8_t boltSeq[]     = {DOWN_BUTTON, UP_BUTTON, A_BUTTON};
    static const uint8_t slideSeq[]    = {LEFT_BUTTON, DOWN_BUTTON, RIGHT_BUTTON, B_BUTTON};
    static const uint8_t screamSeq[]   = {LEFT_BUTTON, RIGHT_BUTTON, A_BUTTON};
    static const uint8_t spinSeq[]     = {DOWN_BUTTON, DOWN_BUTTON | RIGHT_BUTTON, RIGHT_BUTTON, B_BUTTON};
    static const uint8_t mirrorSeq[]   = {RIGHT_BUTTON, LEFT_BUTTON, RIGHT_BUTTON, A_BUTTON};
    static const uint8_t dashSeq[]     = {RIGHT_BUTTON, RIGHT_BUTTON, A_BUTTON};
    
    bool triggered = false;
    uint8_t timer = 15;
    
    switch(player.charIdx) {
        case 0: // ZENITH
            if (checkCombo(fireballSeq, 3)) { player.state = CS_SPECIAL_STARTUP; player.stateTimer = 15; triggered = true; }
            break;
        case 1: // CINDER
            if (checkCombo(uppercutSeq, 3)) { player.state = CS_SPECIAL_STARTUP; player.stateTimer = 10; triggered = true; player.vy = TO_FP(-5); player.isJumping = true; }
            break;
        case 2: // GOLIATH
            if (checkCombo(sumoSeq, 4)) { player.state = CS_SPECIAL_STARTUP; player.stateTimer = 20; triggered = true; player.vx = player.facingLeft ? TO_FP(-3) : TO_FP(3); }
            break;
        case 3: // VOLT
            if (checkCombo(boltSeq, 3)) { player.state = CS_SPECIAL_STARTUP; player.stateTimer = 10; triggered = true; player.vy = TO_FP(-6); player.isJumping = true; }
            break;
        case 4: // KAGE
            if (checkCombo(slideSeq, 4)) { player.state = CS_SPECIAL_STARTUP; player.stateTimer = 12; triggered = true; player.vx = player.facingLeft ? TO_FP(-5) : TO_FP(5); }
            break;
        case 5: // SIREN
            if (checkCombo(screamSeq, 3)) { player.state = CS_SPECIAL_STARTUP; player.stateTimer = 15; triggered = true; }
            break;
        case 6: // DRIFT
            if (checkCombo(hurricaneSeq, 3)) { player.state = CS_SPECIAL_STARTUP; player.stateTimer = 25; triggered = true; player.vx = player.facingLeft ? TO_FP(-2) : TO_FP(2); }
            break;
        case 7: // TUSK
            if (checkCombo(dashSeq, 3)) { player.state = CS_SPECIAL_STARTUP; player.stateTimer = 10; triggered = true; player.vx = player.facingLeft ? TO_FP(-4) : TO_FP(4); }
            break;
        case 8: // JADE
            if (checkCombo(spinSeq, 4)) { player.state = CS_SPECIAL_STARTUP; player.stateTimer = 15; triggered = true; player.vx = player.facingLeft ? TO_FP(-3) : TO_FP(3); }
            break;
        case 9: // ECHO
            if (checkCombo(mirrorSeq, 4)) { player.state = CS_SPECIAL_STARTUP; player.stateTimer = 10; triggered = true; player.x += player.facingLeft ? TO_FP(-40) : TO_FP(40); }
            break;
    }
}

void Game::updateProjectiles() {
    for (uint8_t i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) continue;
        projectiles[i].x += projectiles[i].vx;
        
        // Bounds check
        if (projectiles[i].x < TO_FP(-200) || projectiles[i].x > TO_FP(200)) {
            projectiles[i].active = false;
            continue;
        }
        
        // Collision check
        Skeleton &target = projectiles[i].ownerIsPlayer ? opponent : player;
        int32_t dx = FROM_FP(projectiles[i].x - target.x);
        int32_t dy = FROM_FP(projectiles[i].y - (target.y - TO_FP(10)));
        if (dx*dx + dy*dy < 100) {
            Skeleton &attacker = projectiles[i].ownerIsPlayer ? player : opponent;
            triggerHit(attacker, target, false);
            projectiles[i].active = false;
        }
    }
}

void Game::drawProjectiles() {
    for (uint8_t i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) continue;
        Engine::drawScaledCircle(arduboy, projectiles[i].x, projectiles[i].y, 4, camera, 0);
    }
}

void Game::setup() { 
    arduboy.begin(); 
    arduboy.setFrameRate(60); 
    playerBuffer.head = 0;
    memset(playerBuffer.buttons, 0, INPUT_BUFFER_SIZE);
    for(int i=0; i<MAX_PROJECTILES; i++) projectiles[i].active = false;
}

void Game::loop() { 
    if (!arduboy.nextFrame()) return; 
    arduboy.pollButtons(); 
    arduboy.clear(); 
    switch (currentState) { 
        case STATE_TITLE: drawMenu(); break; 
        case STATE_OPTIONS: drawOptions(); break;
        case STATE_TEST2: drawTest2(); break; 
        case STATE_CHAR_SELECT: drawCharSelect(); break; 
        case STATE_LADDER: drawLadder(); break; 
        case STATE_FIGHT: updateFight(); drawFight(); break; 
        case STATE_ROUND_OVER: drawRoundOver(); break;
        case STATE_RESULTS: 
            arduboy.setCursor(30, 25); 
            arduboy.print(ladderStage == 10 ? F("YOU WIN!") : F("GAME OVER")); 
            if (arduboy.justPressed(A_BUTTON)) currentState = STATE_TITLE; 
            break; 
    } 
    arduboy.display(); 
}

#include <Arduboy2.h>

// --- Fixed-Point Math Constants ---
#define FP_SHIFT 8
#define TO_FP(x) ((int16_t)((x) * (1 << FP_SHIFT)))
#define FROM_FP(x) ((x) >> FP_SHIFT)

// --- Trig Table (256 steps for a full circle) ---
const int16_t SIN_TABLE[64] PROGMEM = {
    0, 6, 12, 18, 25, 31, 37, 43, 49, 56, 62, 68, 74, 80, 86, 92,
    97, 103, 109, 114, 120, 125, 130, 135, 140, 144, 149, 153, 157, 161, 165, 169,
    173, 176, 179, 182, 185, 188, 191, 193, 195, 197, 199, 200, 202, 203, 204, 205,
    206, 206, 207, 207, 207, 207, 207, 207, 206, 206, 205, 204, 203, 202, 200, 199
};

int16_t getSin(uint8_t angle) {
    uint8_t idx = angle & 0x3F;
    uint8_t quad = (angle >> 6) & 0x03;
    int16_t val;
    if (quad == 0) val = pgm_read_word(&SIN_TABLE[idx]);
    else if (quad == 1) val = pgm_read_word(&SIN_TABLE[63 - idx]);
    else if (quad == 2) val = -pgm_read_word(&SIN_TABLE[idx]);
    else val = -pgm_read_word(&SIN_TABLE[63 - idx]);
    return val;
}
int16_t getCos(uint8_t angle) { return getSin(angle + 64); }

// --- Game States ---
enum GameState { STATE_TITLE, STATE_MENU, STATE_CHAR_SELECT, STATE_LADDER, STATE_FIGHT, STATE_RESULTS };

// --- Combat States ---
enum CombatState { 
    CS_IDLE, CS_WALK, CS_BLOCK, CS_ATTACK_STARTUP, CS_ATTACK_ACTIVE, 
    CS_ATTACK_RECOVERY, CS_HITSTUN, CS_PARRY_STUN, CS_SUPER_STARTUP 
};

// --- AI States ---
enum AIState { AI_IDLE, AI_APPROACH, AI_RETREAT, AI_WAIT, AI_ATTACKING };

// --- Character Data ---
struct CharacterData {
    char name[8];
    uint8_t lengths[6]; 
    int16_t walkSpeed;
};

const CharacterData roster[] PROGMEM = {
    {"ZENITH",  {15, 6, 10, 10, 12, 12}, TO_FP(1.2)},
    {"CINDER",  {14, 5, 8, 8, 10, 10},   TO_FP(1.8)},
    {"GOLIATH", {18, 8, 12, 12, 14, 14}, TO_FP(0.8)},
    {"VOLT",    {15, 6, 14, 14, 12, 12}, TO_FP(1.1)},
    {"KAGE",    {15, 6, 9, 9, 11, 11},   TO_FP(1.5)},
    {"SIREN",   {14, 6, 10, 10, 12, 12}, TO_FP(1.2)},
    {"DRIFT",   {15, 6, 10, 10, 12, 12}, TO_FP(1.0)},
    {"TUSK",    {16, 7, 11, 11, 13, 13}, TO_FP(0.9)},
    {"JADE",    {14, 6, 9, 9, 11, 11},   TO_FP(1.4)},
    {"ECHO",    {15, 6, 10, 10, 12, 12}, TO_FP(1.2)}
};

// --- Bone System ---
#define MAX_BONES 12
struct Bone {
    int8_t parent; uint8_t length; uint8_t angle;
    bool isHitbox; bool isHurtbox;
};

struct Skeleton {
    int16_t x, y, vx, vy;
    bool facingLeft, isJumping;
    CombatState state; uint8_t stateTimer;
    int8_t health, special;
    Bone bones[MAX_BONES];
    int16_t worldX[MAX_BONES], worldY[MAX_BONES];
    int16_t walkSpeed;
    AIState aiState; uint8_t aiTimer;
};

// --- Globals ---
Arduboy2 arduboy;
GameState currentState = STATE_TITLE;
struct { int16_t x, y, zoom; } camera = { TO_FP(64), TO_FP(32), 100 };
Skeleton player, opponent;
uint8_t shakeTimer = 0, freezeTimer = 0;
uint8_t selectedChar = 0, ladderStage = 0;

// --- Forward Declarations ---
void updateSkeleton(Skeleton &s);
void drawSkeleton(Skeleton &s);
void initSkeleton(Skeleton &s, uint8_t cIdx, int16_t x, bool faceLeft);
void drawScaledLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

// --- Rendering ---
void drawScaledLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
    int32_t z = camera.zoom;
    int16_t ox = (shakeTimer > 0) ? random(-2, 3) : 0;
    int16_t oy = (shakeTimer > 0) ? random(-2, 3) : 0;
    int16_t sx1 = (int16_t)(((int32_t)(x1 - camera.x) * z) / 100) + TO_FP(64 + ox);
    int16_t sy1 = (int16_t)(((int32_t)(y1 - camera.y) * z) / 100) + TO_FP(32 + oy);
    int16_t sx2 = (int16_t)(((int32_t)(x2 - camera.x) * z) / 100) + TO_FP(64 + ox);
    int16_t sy2 = (int16_t)(((int32_t)(y2 - camera.y) * z) / 100) + TO_FP(32 + oy);
    arduboy.drawLine(FROM_FP(sx1), FROM_FP(sy1), FROM_FP(sx2), FROM_FP(sy2));
}

void updateSkeleton(Skeleton &s) {
    for (uint8_t i = 0; i < MAX_BONES; i++) {
        if (s.bones[i].length == 0 && i > 0) break;
        int16_t startX, startY; uint8_t angle = s.bones[i].angle;
        if (s.state == CS_ATTACK_ACTIVE || s.state == CS_ATTACK_STARTUP) if (i == 2) angle = 0;
        if (s.state == CS_SUPER_STARTUP) if (i == 2) angle = 192;
        if (s.facingLeft) angle = 128 - angle;
        if (s.bones[i].parent == -1) { startX = s.x; startY = s.y; }
        else { startX = s.worldX[s.bones[i].parent]; startY = s.worldY[s.bones[i].parent]; }
        s.worldX[i] = startX + TO_FP((getCos(angle) * s.bones[i].length) >> 8);
        s.worldY[i] = startY + TO_FP((getSin(angle) * s.bones[i].length) >> 8);
    }
}

void drawSkeleton(Skeleton &s) {
    for (uint8_t i = 0; i < MAX_BONES; i++) {
        if (s.bones[i].length == 0 && i > 0) break;
        int16_t startX, startY;
        if (s.bones[i].parent == -1) { startX = s.x; startY = s.y; }
        else { startX = s.worldX[s.bones[i].parent]; startY = s.worldY[s.bones[i].parent]; }
        drawScaledLine(startX, startY, s.worldX[i], s.worldY[i]);
    }
}

void initSkeleton(Skeleton &s, uint8_t cIdx, int16_t x, bool faceLeft) {
    s.x = x; s.y = TO_FP(55); s.vx = 0; s.vy = 0; s.facingLeft = faceLeft;
    s.isJumping = false; s.state = CS_IDLE; s.stateTimer = 0; s.health = 100; s.special = 0;
    s.aiState = AI_IDLE; s.aiTimer = 0;
    CharacterData data; memcpy_P(&data, &roster[cIdx], sizeof(CharacterData));
    s.walkSpeed = data.walkSpeed;
    s.bones[0] = {-1, data.lengths[0], 192, false, true};
    s.bones[1] = {0,  data.lengths[1], 192, false, true};
    s.bones[2] = {0,  data.lengths[2], 32,  true,  true};
    s.bones[3] = {0,  data.lengths[3], 160, true,  true};
    s.bones[4] = {-1, data.lengths[4], 64,  false, true};
    s.bones[5] = {-1, data.lengths[5], 128, false, true};
    for(int i=6; i<MAX_BONES; i++) s.bones[i].length = 0;
}

// --- Combat Logic ---
#define GROUND_Y TO_FP(55)
#define GRAVITY TO_FP(0.2)
#define JUMP_IMPULSE TO_FP(-4)

void triggerHit(Skeleton &attacker, Skeleton &defender, bool isSuper = false) {
    shakeTimer = isSuper ? 20 : 8; freezeTimer = isSuper ? 15 : 5;
    
    // Parry check
    uint8_t fwd = defender.facingLeft ? LEFT_BUTTON : RIGHT_BUTTON;
    if (arduboy.pressed(fwd) && defender.stateTimer < 8) {
        defender.state = CS_IDLE; defender.special += 25;
        attacker.state = CS_PARRY_STUN; attacker.stateTimer = 40;
        return;
    }

    int8_t dmg = isSuper ? 30 : 10;
    if (defender.state == CS_BLOCK) {
        defender.health -= (dmg / 4); defender.special += 2; attacker.special += 5;
    } else {
        defender.health -= dmg; defender.state = CS_HITSTUN; defender.stateTimer = isSuper ? 30 : 15;
        defender.special += 5; attacker.special += 10;
    }
    if (defender.health < 0) defender.health = 0;
    if (defender.special > 100) defender.special = 100;
    if (attacker.special > 100) attacker.special = 100;
}

void updateAI() {
    if (opponent.stateTimer > 0) return; 
    if (opponent.aiTimer > 0) { opponent.aiTimer--; return; }

    int16_t dist = abs(FROM_FP(player.x - opponent.x));
    uint8_t aggression = 20 + (ladderStage * 8); 
    
    switch (opponent.aiState) {
        case AI_IDLE:
            opponent.aiState = (dist > 45) ? AI_APPROACH : AI_WAIT;
            opponent.aiTimer = random(5, 15 - ladderStage);
            break;
        case AI_APPROACH:
            opponent.vx = opponent.facingLeft ? -opponent.walkSpeed : opponent.walkSpeed;
            if (dist < 40) { opponent.aiState = AI_ATTACKING; }
            break;
        case AI_WAIT:
            opponent.vx = 0;
            if (random(0, 100) < aggression) opponent.aiState = AI_ATTACKING;
            else if (dist < 30) opponent.aiState = AI_RETREAT;
            opponent.aiTimer = random(10, 20);
            break;
        case AI_RETREAT:
            opponent.vx = opponent.facingLeft ? opponent.walkSpeed : -opponent.walkSpeed;
            if (dist > 60 || random(0, 10) == 0) opponent.aiState = AI_IDLE;
            break;
        case AI_ATTACKING:
            opponent.vx = 0;
            opponent.state = CS_ATTACK_STARTUP;
            opponent.stateTimer = 10 - (ladderStage / 2);
            opponent.aiState = AI_IDLE;
            break;
    }
}

void updateFight() {
    if (freezeTimer > 0) { freezeTimer--; return; }
    if (shakeTimer > 0) shakeTimer--;
    
    // Player Logic
    if (player.stateTimer > 0) player.stateTimer--;
    if (player.state <= CS_BLOCK) {
        player.vx = 0; uint8_t back = player.facingLeft?RIGHT_BUTTON:LEFT_BUTTON, fwd = player.facingLeft?LEFT_BUTTON:RIGHT_BUTTON;
        if (arduboy.pressed(back)) { player.state = CS_BLOCK; }
        else if (arduboy.pressed(fwd)) { player.vx = player.facingLeft?-player.walkSpeed:player.walkSpeed; player.state = CS_WALK; }
        else player.state = CS_IDLE;
        if (arduboy.justPressed(UP_BUTTON) && !player.isJumping) { player.vy = JUMP_IMPULSE; player.isJumping = true; }
        
        if (arduboy.pressed(A_BUTTON) && arduboy.pressed(B_BUTTON) && player.special >= 100) {
            player.state = CS_SUPER_STARTUP; player.stateTimer = 40; player.special = 0; freezeTimer = 40;
        } else if (arduboy.justPressed(A_BUTTON)) {
            player.state = CS_ATTACK_STARTUP; player.stateTimer = 8;
        }
    } else if (player.stateTimer == 0) {
        if (player.state == CS_SUPER_STARTUP) { player.state = CS_ATTACK_ACTIVE; player.stateTimer = 20; }
        else if (player.state == CS_ATTACK_STARTUP) { player.state = CS_ATTACK_ACTIVE; player.stateTimer = 12; }
        else if (player.state == CS_ATTACK_ACTIVE) { player.state = CS_ATTACK_RECOVERY; player.stateTimer = 10; }
        else player.state = CS_IDLE;
    }

    // Opponent logic (AI)
    if (opponent.stateTimer > 0) opponent.stateTimer--;
    if (opponent.state <= CS_BLOCK) {
        updateAI();
    } else if (opponent.stateTimer == 0) {
        if (opponent.state == CS_ATTACK_STARTUP) { opponent.state = CS_ATTACK_ACTIVE; opponent.stateTimer = 12; }
        else if (opponent.state == CS_ATTACK_ACTIVE) { opponent.state = CS_ATTACK_RECOVERY; opponent.stateTimer = 10; }
        else opponent.state = CS_IDLE;
    }

    player.facingLeft = (player.x > opponent.x); opponent.facingLeft = !player.facingLeft;
    player.vy += GRAVITY; player.x += player.vx; player.y += player.vy; if (player.y >= GROUND_Y) { player.y = GROUND_Y; player.vy = 0; player.isJumping = false; }
    opponent.vy += GRAVITY; opponent.x += opponent.vx; opponent.y += opponent.vy; if (opponent.y >= GROUND_Y) { opponent.y = GROUND_Y; opponent.vy = 0; opponent.isJumping = false; }
    
    updateSkeleton(player); updateSkeleton(opponent);

    // Collision Player -> Opponent
    if (player.state == CS_ATTACK_ACTIVE) {
        for(uint8_t i=0; i<MAX_BONES; i++) if (player.bones[i].isHitbox) {
            for(uint8_t j=0; j<MAX_BONES; j++) if (opponent.bones[j].isHurtbox) {
                int32_t dx = FROM_FP(player.worldX[i] - opponent.worldX[j]), dy = FROM_FP(player.worldY[i] - opponent.worldY[j]);
                if (dx*dx + dy*dy < 16) triggerHit(player, opponent, (player.stateTimer > 15));
            }
        }
    }
    // Collision Opponent -> Player
    if (opponent.state == CS_ATTACK_ACTIVE) {
        for(uint8_t i=0; i<MAX_BONES; i++) if (opponent.bones[i].isHitbox) {
            for(uint8_t j=0; j<MAX_BONES; j++) if (player.bones[j].isHurtbox) {
                int32_t dx = FROM_FP(opponent.worldX[i] - player.worldX[j]), dy = FROM_FP(opponent.worldY[i] - player.worldY[j]);
                if (dx*dx + dy*dy < 16) triggerHit(opponent, player);
            }
        }
    }

    int16_t centerX = (player.x + opponent.x) / 2, centerY = (player.y + opponent.y) / 2 - TO_FP(10);
    camera.x = centerX; camera.y = centerY;
    int32_t dist = abs(FROM_FP(player.x - opponent.x)); if (dist < 30) dist = 30;
    camera.zoom = 4000 / dist; if (camera.zoom > 150) camera.zoom = 150; if (camera.zoom < 60) camera.zoom = 60;

    if (opponent.health == 0) { currentState = STATE_LADDER; ladderStage++; }
    if (player.health == 0) { currentState = STATE_RESULTS; }
}

void drawFight() {
    drawScaledLine(TO_FP(-500), GROUND_Y, TO_FP(500), GROUND_Y);
    drawSkeleton(player); drawSkeleton(opponent);
    
    // Health bars
    arduboy.drawRect(2, 2, 52, 5, WHITE); arduboy.fillRect(3, 3, player.health/2, 3, WHITE);
    arduboy.drawRect(74, 2, 52, 5, WHITE); arduboy.fillRect(75 + (50 - opponent.health/2), 3, opponent.health/2, 3, WHITE);
    
    // Special bar (Player only)
    arduboy.drawRect(2, 58, 42, 4, WHITE); arduboy.fillRect(3, 59, player.special * 40 / 100, 2, WHITE);
}

void drawCharSelect() {
    arduboy.setCursor(30, 5); arduboy.print(F("SELECT HERO"));
    for(uint8_t i=0; i<10; i++) {
        uint8_t x = 10 + (i%5)*22, y = 20 + (i/5)*20;
        arduboy.drawRect(x, y, 18, 16, (selectedChar == i) ? WHITE : BLACK);
        if (selectedChar == i) {
            CharacterData d; memcpy_P(&d, &roster[i], sizeof(CharacterData));
            arduboy.setCursor(40, 50); arduboy.print(d.name);
        }
    }
    if (arduboy.justPressed(LEFT_BUTTON) && selectedChar > 0) selectedChar--;
    if (arduboy.justPressed(RIGHT_BUTTON) && selectedChar < 9) selectedChar++;
    if (arduboy.justPressed(A_BUTTON)) { ladderStage = 0; currentState = STATE_LADDER; }
}

void drawLadder() {
    arduboy.setCursor(40, 5); arduboy.print(F("LADDER"));
    for(uint8_t i=0; i<10; i++) {
        uint8_t y = 55 - (i*5); arduboy.drawFastHLine(50, y, 28, WHITE);
        if (ladderStage == i) arduboy.setCursor(30, y-3), arduboy.print(F(">"));
    }
    if (ladderStage >= 10) currentState = STATE_RESULTS;
    else if (arduboy.justPressed(A_BUTTON)) {
        initSkeleton(player, selectedChar, TO_FP(30), false);
        initSkeleton(opponent, ladderStage, TO_FP(100), true);
        currentState = STATE_FIGHT;
    }
}

void setup() { arduboy.begin(); arduboy.setFrameRate(60); }
void loop() {
    if (!arduboy.nextFrame()) return;
    arduboy.pollButtons(); arduboy.clear();
    switch (currentState) {
        case STATE_TITLE:
            arduboy.setCursor(25, 25); arduboy.print(F("STICK FIGHTER"));
            if (arduboy.justPressed(A_BUTTON)) currentState = STATE_CHAR_SELECT;
            break;
        case STATE_CHAR_SELECT: drawCharSelect(); break;
        case STATE_LADDER: drawLadder(); break;
        case STATE_FIGHT: updateFight(); drawFight(); break;
        case STATE_RESULTS:
            arduboy.setCursor(30, 25); arduboy.print(ladderStage == 10 ? F("YOU WIN!") : F("GAME OVER"));
            if (arduboy.justPressed(A_BUTTON)) currentState = STATE_TITLE;
            break;
    }
    arduboy.display();
}

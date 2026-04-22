#ifndef GAME_DATA_H
#define GAME_DATA_H

#ifdef ARDUINO
#include <Arduino.h>
#endif

// --- Fixed-Point Math Constants ---
#define FP_SHIFT 8
#define TO_FP(x) ((int32_t)((x) * 256L))
#define FROM_FP(x) ((int32_t)(x) >> 8)

// --- Trig Table ---
const int16_t SIN_TABLE[64] PROGMEM = {
    0, 6, 12, 18, 25, 31, 37, 43, 49, 56, 62, 68, 74, 80, 86, 92,
    97, 103, 109, 114, 120, 125, 130, 135, 140, 144, 149, 153, 157, 161, 165, 169,
    173, 176, 179, 182, 185, 188, 191, 193, 195, 197, 199, 200, 202, 203, 204, 205,
    206, 206, 207, 207, 207, 207, 207, 207, 206, 206, 205, 204, 203, 202, 200, 199
};

// --- Game States ---
enum GameState { STATE_TITLE, STATE_OPTIONS, STATE_TEST, STATE_TEST2, STATE_CHAR_SELECT, STATE_LADDER, STATE_FIGHT, STATE_ROUND_OVER, STATE_RESULTS };

// --- Combat States ---
enum CombatState { CS_IDLE, CS_WALK, CS_BLOCK, CS_DUCK, CS_PUNCH_STARTUP, CS_PUNCH_ACTIVE, CS_PUNCH_RECOVERY, CS_KICK_STARTUP, CS_KICK_ACTIVE, CS_KICK_RECOVERY, CS_HITSTUN, CS_PARRY_STUN, CS_SUPER_STARTUP, CS_DUCK_PUNCH_STARTUP, CS_DUCK_PUNCH_ACTIVE, CS_DUCK_PUNCH_RECOVERY, CS_DUCK_KICK_STARTUP, CS_DUCK_KICK_ACTIVE, CS_DUCK_KICK_RECOVERY };

// --- AI States ---
enum AIState { AI_IDLE, AI_APPROACH, AI_RETREAT, AI_WAIT, AI_ATTACKING };

// --- Constants ---
#define GROUND_Y TO_FP(55)
#define GRAVITY TO_FP(0.2)
#define JUMP_IMPULSE TO_FP(-4.5)
#define ACCEL TO_FP(0.3)
#define FRICTION TO_FP(0.2)

// --- Face Data ---
struct FaceData { uint8_t headShape, hairStyle, eyeType, noseType, mouthType, browType; };

// --- Poses ---
struct Pose { uint8_t angles[6]; };

const Pose poses[] PROGMEM = {
    {{194, 190, 95, 35, 75, 41}},   // 0: IDLE
    {{198, 190, 25, 100, 40, 85}},  // 1: WALK 1
    {{194, 190, 64, 64, 64, 64}},   // 2: WALK 2
    {{190, 190, 100, 25, 85, 40}},  // 3: WALK 3
    {{194, 190, 64, 64, 64, 64}},   // 4: WALK 4
    {{192, 192, 234, 38, 98, 46}},  // 5: BLOCK
    {{220, 205, 66, 238, 64, 34}},  // 6: PUNCH ACTIVE
    {{192, 210, 80, 42, 84, 242}},  // 7: KICK ACTIVE
    {{192, 2, 54, 36, 96, 20}},     // 8: DUCK
    {{172, 168, 22, 30, 46, 30}},   // 9: HITSTUN
    {{192, 2, 40, 242, 64, 42}},     // 10: DUCK PUNCH
    {{192, 2, 74, 54, 56, 15}}     // 11: DUCK KICK
};

const char* const animNames[] = { "IDLE", "W1", "W2", "W3", "W4", "BLOCK", "PUNCH", "KICK", "DUCK", "HIT", "DPUN", "DKIC" };

struct CharacterData { char name[8]; uint8_t lengths[6]; int16_t walkSpeed; FaceData face; };

const CharacterData roster[] PROGMEM = {
    {"ZENITH",  {12, 6, 10, 10, 12, 12}, TO_FP(1.3), {0, 1, 0, 2, 0, 1}},
    {"CINDER",  {11, 5, 8, 8, 10, 10},   TO_FP(1.9), {2, 3, 2, 1, 1, 2}},
    {"GOLIATH", {15, 8, 12, 12, 14, 14}, TO_FP(0.9), {1, 0, 1, 2, 2, 1}},
    {"VOLT",    {12, 6, 14, 14, 12, 12}, TO_FP(1.2), {0, 4, 3, 0, 1, 0}},
    {"KAGE",    {12, 6, 9, 9, 11, 11},   TO_FP(1.6), {2, 1, 2, 1, 0, 2}},
    {"SIREN",   {11, 6, 10, 10, 12, 12}, TO_FP(1.3), {0, 2, 0, 1, 1, 0}},
    {"DRIFT",   {12, 6, 10, 10, 12, 12}, TO_FP(1.1), {2, 4, 3, 2, 0, 1}},
    {"TUSK",    {13, 7, 11, 11, 13, 13}, TO_FP(1.0), {1, 1, 2, 2, 2, 2}},
    {"JADE",    {11, 6, 9, 9, 11, 11},   TO_FP(1.5), {0, 2, 1, 1, 1, 1}},
    {"ECHO",    {12, 6, 10, 10, 12, 12}, TO_FP(1.3), {2, 0, 0, 0, 0, 0}}
};

#define MAX_BONES 12
struct Bone { int8_t parent; uint8_t length; bool isHitbox; bool isHurtbox; };
struct Skeleton { int32_t x, y; int16_t vx, vy; bool facingLeft, isJumping; CombatState state; uint8_t stateTimer; int8_t health, special; Bone bones[MAX_BONES]; uint8_t currentAngles[MAX_BONES]; int32_t worldX[MAX_BONES], worldY[MAX_BONES]; int16_t walkSpeed; AIState aiState; uint8_t aiTimer; uint8_t charIdx; uint8_t breathingPhase; };

#endif

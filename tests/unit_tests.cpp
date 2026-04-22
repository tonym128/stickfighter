#define Arduboy2_h // Block real Arduboy2.h
#include "mock_arduboy.h"

// Redefine standard Arduboy macros for the test environment
#undef Arduboy2_h 
#define Arduboy2_h 

#include "../stickfighter.ino"

#include <assert.h>
#include <stdio.h>

void test_fixed_point() {
    printf("Testing Fixed Point Math... ");
    int16_t fp = TO_FP(10);
    assert(FROM_FP(fp) == 10);
    
    // Check float-like precision
    int16_t fp2 = TO_FP(1.5);
    assert(FROM_FP(fp2) == 1);
    assert(fp2 == 0x0180); // 1.5 in 8.8 is 384 or 0x0180
    printf("PASSED\n");
}

void test_collision() {
    printf("Testing Collision Logic... ");
    Skeleton a, b;
    initSkeleton(a, 0, TO_FP(10), false);
    initSkeleton(b, 0, TO_FP(12), true); // Very close
    
    a.state = CS_ATTACK_ACTIVE;
    updateSkeleton(a);
    updateSkeleton(b);
    
    // They should collide at this distance
    bool hit = false;
    for(uint8_t i=0; i<MAX_BONES; i++) {
        if (a.bones[i].isHitbox) {
            for(uint8_t j=0; j<MAX_BONES; j++) {
                if (b.bones[j].isHurtbox) {
                    int32_t dx = FROM_FP(a.worldX[i] - b.worldX[j]);
                    int32_t dy = FROM_FP(a.worldY[i] - b.worldY[j]);
                    if (dx*dx + dy*dy < 16) hit = true;
                }
            }
        }
    }
    assert(hit == true);
    printf("PASSED\n");
}

void test_combat_state() {
    printf("Testing Combat Transitions... ");
    Skeleton s;
    initSkeleton(s, 0, TO_FP(50), false);
    
    assert(s.health == 100);
    assert(s.state == CS_IDLE);
    
    // Simulate being hit
    Skeleton attacker;
    initSkeleton(attacker, 0, TO_FP(10), false);
    attacker.state = CS_ATTACK_ACTIVE;
    
    triggerHit(attacker, s, false);
    assert(s.health == 90);
    assert(s.state == CS_HITSTUN);
    assert(s.stateTimer == 15);
    printf("PASSED\n");
}

void test_parry() {
    printf("Testing Parry Logic... ");
    Skeleton p, o;
    initSkeleton(p, 0, TO_FP(50), false); // Facing Right
    initSkeleton(o, 0, TO_FP(60), true);  // Facing Left
    
    // Player holds Right (Forward for P1) to parry
    arduboy.btns = RIGHT_BUTTON;
    p.stateTimer = 0; // Fresh frame
    
    triggerHit(o, p, false);
    
    assert(p.health == 100); // No damage on parry
    assert(p.special == 25); // Gain special
    assert(o.state == CS_PARRY_STUN); // Attacker stunned
    printf("PASSED\n");
}

int main() {
    printf("--- STICKFIGHTER UNIT TESTS ---\n");
    test_fixed_point();
    test_collision();
    test_combat_state();
    test_parry();
    printf("-------------------------------\n");
    printf("ALL TESTS PASSED SUCCESSFULLY!\n");
    return 0;
}

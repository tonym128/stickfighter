#include "../Platform.h"
#include "../Game.h"
#include "../Engine.h"
#include <assert.h>
#include <stdio.h>

// A subclass to expose protected members for testing
class TestGame : public Game {
public:
    using Game::player;
    using Game::opponent;
    using Game::triggerHit;
    using Game::arduboy;
    using Game::editablePose;
};

void test_fixed_point() {
    printf("Testing Fixed Point Math... ");
    int32_t fp = TO_FP(10);
    assert(FROM_FP(fp) == 10);
    
    // Check float-like precision
    int32_t fp2 = TO_FP(1.5);
    assert(FROM_FP(fp2) == 1);
    assert(fp2 == 0x0180); // 1.5 in 8.8 is 384 or 0x0180
    printf("PASSED\n");
}

void test_collision() {
    printf("Testing Collision Logic... ");
    TestGame game;
    Engine::initSkeleton(game.player, 0, TO_FP(10), false);
    Engine::initSkeleton(game.opponent, 0, TO_FP(12), true); // Very close
    
    game.player.state = CS_PUNCH_ACTIVE;
    Engine::updateSkeleton(game.player, game.editablePose, 0, 6);
    Engine::updateSkeleton(game.opponent, game.editablePose, 0, 0);
    
    // They should collide at this distance
    bool hit = false;
    for(uint8_t i=0; i<MAX_BONES; i++) {
        if (game.player.bones[i].isHitbox) {
            for(uint8_t j=0; j<MAX_BONES; j++) {
                if (game.opponent.bones[j].isHurtbox) {
                    int32_t dx = FROM_FP(game.player.worldX[i] - game.opponent.worldX[j]);
                    int32_t dy = FROM_FP(game.player.worldY[i] - game.opponent.worldY[j]);
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
    TestGame game;
    Engine::initSkeleton(game.player, 0, TO_FP(50), false);
    
    assert(game.player.health == 100);
    assert(game.player.state == CS_IDLE);
    
    // Simulate being hit
    Skeleton attacker;
    Engine::initSkeleton(attacker, 0, TO_FP(10), false);
    attacker.state = CS_PUNCH_ACTIVE;
    
    game.triggerHit(attacker, game.player, false);
    assert(game.player.health == 90);
    assert(game.player.state == CS_HITSTUN);
    assert(game.player.stateTimer == 15);
    printf("PASSED\n");
}

int main() {
    printf("--- STICKFIGHTER UNIT TESTS ---\n");
    test_fixed_point();
    test_collision();
    test_combat_state();
    printf("-------------------------------\n");
    printf("ALL TESTS PASSED\n");
    return 0;
}

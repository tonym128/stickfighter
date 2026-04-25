#ifndef ARDUINO
#include "../Game.h"

int main(int argc, char* argv[]) {
    while (true) {
        Game game;
        game.setup();
        Arduboy2::shouldRestart = false;
        Arduboy2::frameCount = 0;
        while (!Arduboy2::shouldRestart) {
            game.loop();
        }
    }
    return 0;
}
#endif

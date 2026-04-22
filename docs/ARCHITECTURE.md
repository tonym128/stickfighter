# StickFighter Architecture

## Technical Constraints (Arduboy)
- **CPU:** ATmega32u4 (8-bit, 16MHz)
- **RAM:** 2.5 KB (Extremely limited)
- **Flash:** 32 KB
- **Display:** 128x64 pixels, Monochrome

## System Components

### 1. Global State Machine
Manages transitions between game modes:
- `STATE_TITLE`: Splash screen and menu.
- `STATE_CHAR_SELECT`: Character picking.
- `STATE_LADDER`: Displaying the "Tower" of opponents.
- `STATE_FIGHT`: The core gameplay loop.
- `STATE_PAUSE`: Mid-fight options.
- `STATE_RESULTS`: Win/Loss screen.

### 2. Memory Management
- **Static Allocation:** Avoid `malloc` to prevent heap fragmentation.
- **PROGMEM:** Store character move data, bone definitions, and strings in Flash memory.
- **Buffer Management:** Use a single screen buffer (1024 bytes) provided by `Arduboy2`.

### 3. Entity System
Two primary entities: `Player` and `Opponent`.
Each contains:
- Current state (Idle, Attacking, Hit, Block, Jump, Duck).
- Bone positions/angles.
- Health/Special meter values.
- Physics (Velocity, Gravity).

### 4. Input Buffer
A small circular buffer to store the last 8-10 frames of input to detect combo sequences (e.g., Down -> Forward -> A).

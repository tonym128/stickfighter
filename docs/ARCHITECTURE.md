# StickFighter Architecture

## Technical Constraints (Arduboy)
- **CPU:** ATmega32u4 (8-bit, 16MHz)
- **RAM:** 2.5 KB (Extremely limited)
- **Flash:** 32 KB
- **Display:** 128x64 pixels, Monochrome

## System Components

### 1. Platform-Agnostic Engine
The core logic is divided into three layers:
- **Platform.h**: An abstraction layer that emulates the `Arduboy2` interface using **SDL2** for local development.
- **Engine.cpp/h**: Handles platform-independent math, fixed-point arithmetic, bone transformations, and rendering primitives.
- **Game.cpp/h**: Manages the high-level game state, AI, and combat logic.

### 2. State Machine
Manages transitions between game modes:
- `STATE_TITLE`: Logo display and main menu.
- `STATE_CHAR_SELECT`: Hero selection.
- `STATE_LADDER`: Opponent progression.
- `STATE_FIGHT`: The core 1v1 combat loop.
- `STATE_ROUND_OVER`: Visual feedback between rounds.
- `STATE_RESULTS`: Win/Loss finality.

### 3. Memory Management
- **Static Allocation:** No heap usage (`malloc`/`new` only used in SDL emulator setup).
- **PROGMEM Optimization:** Roster data, trig tables, and animation poses are stored in Flash memory.
- **Bone Hierarchy:** Recursive skeletal system allows complex animations with minimal RAM.

### 4. Input Buffer
A circular 60-frame buffer stores every button state, enabling a lenient motion-search combo parser for special moves.

# StickFighter

A high-performance monochrome fighting game for the **Arduboy** and **SDL2**.

## Gameplay

StickFighter is a technical fighting game featuring 10 unique characters, each with their own bone-based animation set and special moves. Fight through a ladder of opponents to become the champion!

### Controls

| Action | Arduboy | SDL2 (Keyboard) |
| :--- | :--- | :--- |
| **Move Left** | Left | Left Arrow |
| **Move Right** | Right | Right Arrow |
| **Jump** | Up | Up Arrow |
| **Duck** | Down | Down Arrow |
| **Punch (A)** | A | Z |
| **Kick (B)** | B | X |
| **Block** | Hold Back | Hold Back |
| **Quit** | - | Q |
| **Restart** | - | R |

## Special Moves

Special moves are activated using a specific directional sequence followed by an attack button. **All characters share the same special move.**

**Cost:** 50% Special Meter

| Special Move | Combo (Facing Right) | Description |
| :--- | :--- | :--- |
| **Fireball** | ↓ → + A | Launch a projectile across the screen. |

## Development

The game is written in C++ with a platform-agnostic engine.

### Building for SDL2 (Local Testing)
```bash
make sdl
./stickfighter_sdl
```

### Building for Arduboy
```bash
make compile
```

### Animation Editor
The project includes a multi-window desktop animation editor for fine-tuning character poses.
```bash
make editor
./stickfighter_editor
```
- **Preview Window**: Shows the animation playing at 60 FPS.
- **Controls Window**: Select poses, bones, and adjust angles via mouse.
- **Save**: Click "SAVE" to update `poses.h` directly.

## Architecture
- **Engine.cpp/h**: Core math, physics, and skeletal rendering.
- **Game.cpp/h**: Main game loop, AI, and combat logic.
- **Platform.h**: Compatibility layer between Arduino/Arduboy and SDL2.
- **game_data.h**: Character stats, constants, and global structures.
- **poses.h**: Character pose data (Flash memory optimized).

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

## Recent Updates: "Greatness" Update

### 1. Narrative Expansion
*   **Story Mode Integration:** Every fighter now features a unique **Intro** and **Ending** sequence. These character-specific stories provide motivation at the start of the ladder and a sense of "fulfilling greatness" upon victory.
*   **Dynamic Presentation:** Stories are displayed with character-themed screens and animated prompts to enhance the arcade experience.

### 2. Tactical AI Overhaul
*   **Archetype System:** AI now follows specific profiles: **Rushdown** (aggressive), **Zoners** (projectile-focused), and **Tanks** (patient counter-attackers).
*   **Anti-Cheese Mechanisms:** The AI now recognizes camping tactics. If a player stays ducking, the AI will close the gap and prioritize low-breaking attacks.
*   **Skill-Based Parrying:** The parry system has been tightened to require frame-perfect timing, preventing "auto-parry" exploits.

### 3. Physical Archetype Scaling
*   **Dynamic Stats:** Character size (skeletal limb length) now fundamentally alters gameplay:
    *   **Bigger (Heavies):** Higher damage output, reduced movement speed.
    *   **Smaller (Lights):** High mobility and speed, reduced raw power.
*   **Roster Rebalancing:** The 10-character roster has been retuned to balance speed and power against physical reach.

### 4. Developer Tools & Engine Stability
*   **Editor Zoom Suite:** The Animation Editor now features a comprehensive zoom system. Use **Up/Down arrows** or the new **UI +/- buttons** to inspect bone alignments at any scale.
*   **Intelligent Camera:** The editor camera now dynamically recalculates its height based on zoom level, ensuring the character remains centered and the ground line stays visible.
*   **Platform Multi-Window Fix:** Resolved a critical memory corruption issue in the SDL mock layer. The engine now properly supports independent memory buffers for multiple `Arduboy2` instances, enabling stable multi-window tools.
*   **Engine Refinement:** Facial features now maintain consistent artistic proportions across all zoom levels through a new threshold-based scaling system.

## License
MIT.

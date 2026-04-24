# Project Overview
`stickfighter` is a technical fighting game developed for the Arduboy (monochrome handheld) and SDL2 (Desktop). It uses a custom-built skeletal engine for high-performance 60fps animations.

## Main Technologies
- **Platform:** Arduboy (ATmega32u4) & SDL2
- **Language:** C++
- **Primary Library:** `Arduboy2` (Emulated on Desktop)

## Building and Running

### Using the Makefile (Recommended)
1.  **Arduboy (Binary)**: Run `make compile` to generate the `.hex` file.
2.  **SDL2 (Game)**: Run `make sdl` then `./stickfighter_sdl`.
3.  **SDL2 (Editor)**: Run `make editor` then `./stickfighter_editor`.
4.  **Tests**: Run `make test`.

### Dependencies
- **Arduboy**: `arduino-cli` with `arduboy:avr` core installed.
- **Desktop**: `libsdl2-dev` installed.

## Release State
- **Characters**: 10 distinct fighters.
- **Specials**: Fireball (↓ → + A) universal to all characters.
- **License**: MIT.
- **Optimization**: Core engine is platform-agnostic; architecture split into `Engine`, `Game`, and `Platform` layers.

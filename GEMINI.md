# Project Overview
`stickfighter` is an Arduboy game project. It is written in C++ using the [Arduboy2 library](https://github.com/MLXXXP/Arduboy2). The project follows the standard Arduino sketch structure, optimized for the Arduboy handheld console's hardware.

## Main Technologies
- **Platform:** Arduboy (ATmega32u4)
- **Language:** C++ (Arduino)
- **Primary Library:** `Arduboy2`

## Building and Running
To build and run this project, you can use the Arduino IDE or the provided `Makefile` with `arduino-cli`.

### Using the CLI (Recommended)
1.  **Install `arduino-cli`**: Follow the [official installation guide](https://arduino.github.io/arduino-cli/latest/installation/).
2.  **Setup Environment**: Run `make setup` to install the necessary board cores and libraries.
3.  **Compile**: Run `make` to generate the `.hex` file in the `build/` directory.
4.  **Upload**: Connect your Arduboy and run `make upload` (you may need to adjust the port in the `Makefile`).

### Using the Arduino IDE
1.  **Dependencies:** Ensure the `Arduboy2` library is installed via the Arduino Library Manager.
2.  **Board Selection:** Select "Arduboy" or "Arduino Leonardo" as the board.
3.  **Upload:** Use the standard Upload command to flash the `stickfighter.ino` sketch.

## Development Conventions
- **Frame Rate:** The project is configured to run at 60 FPS using `arduboy.setFrameRate(60)`.
- **Main Loop:** Logic should be placed within the `loop()` function, ensuring `arduboy.nextFrame()` is called to maintain consistent timing.
- **Input Handling:** Use `arduboy.pollButtons()` and associated button state checks (e.g., `arduboy.pressed(UP_BUTTON)`) for user interaction.
- **Rendering:** All drawing calls should occur between `arduboy.clear()` and `arduboy.display()`.

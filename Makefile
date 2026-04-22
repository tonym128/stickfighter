# StickFighter Makefile for arduino-cli

SKETCH = stickfighter.ino
BOARD = arduboy:avr:arduboy
# If the Arduboy board package is not installed, use Leonardo:
# BOARD = arduino:avr:leonardo
BUILD_DIR = build
OUTPUT_HEX = $(BUILD_DIR)/stickfighter.ino.hex

.PHONY: all setup compile upload clean test

all: compile

test:
	g++ -I./tests tests/unit_tests.cpp -o tests/runner
	./tests/runner

setup:
	./bin/arduino-cli core update-index
	./bin/arduino-cli core install arduboy:avr
	./bin/arduino-cli lib install Arduboy2

compile:
	mkdir -p $(BUILD_DIR)
	./bin/arduino-cli compile --fqbn $(BOARD) --output-dir $(BUILD_DIR) $(SKETCH)

upload:
	./bin/arduino-cli upload -p /dev/ttyACM0 --fqbn $(BOARD) --input-dir $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

# StickFighter Implementation Roadmap

## Phase 1: Foundation & Scaffolding
- [x] Initialize `stickfighter.ino` with `Arduboy2` boilerplate.
- [x] Implement a basic Global State Machine (Title, Fight, Menu).
- [x] Create a Fixed-Point Math utility (avoiding `float` for performance).
- [x] Set up the Camera system (X, Y, and Zoom variables).

## Phase 2: The Bone Engine (Core Tech)
- [x] Define the `Bone` and `Skeleton` structures.
- [x] Implement recursive bone rendering (Parent -> Child rotation).
- [x] Implement `drawScaledLine` and `drawScaledCircle` functions.
- [x] Create a simple "Stick Man" pose and verify it scales correctly with Camera Zoom.
- [ ] Implement a Keyframe interpolation system for smooth movement. (Deferred to Phase 3/5 for integration with state machine)

## Phase 3: Physics & Basic Movement
- [x] Implement Ground Detection and Gravity.
- [x] Add Left/Right walking with boundary checks.
- [x] Add Jumping logic (Up button).
- [x] Implement "Face-to-Face" logic (Characters always turn to face each other).
- [x] Implement Dynamic Zoom (Zoom = 100 / distance between players, clamped).

## Phase 4: Input & Combat Logic
- [x] Create a Circular Input Buffer (stores last 30 frames of button states).
- [x] Implement Attack States (Idle -> Startup -> Active -> Recovery -> Idle).
- [x] Implement Bone-to-Bone Collision (Check distance between "Attacking" bones and "Hurt" bones).
- [x] Implement Blocking logic (Hold Back to enter Block state).
- [x] Implement Parry Window (Logic structure ready, refined in Phase 5).

## Phase 5: Advanced Combat & Specials
- [x] Implement Combo Parser (detects patterns like `Down, Forward, A` in buffer).
- [x] Create the Special Meter logic (gains on hit/parry).
- [x] Implement "Super" move freeze-frame effect.
- [x] Add Screen Shake on heavy hits.
- [x] Refine Parry mechanism (Forward relative to facing).

## Phase 6: UI & Character Data
- [x] Design and render Health Bars (using `drawFastHLine`).
- [x] Design and render Special Meters.
- [x] Implement PROGMEM storage for Character 1 (Zenith) stats and move sets.
- [x] Implement Character Selection Screen (Grid of 10).
- [x] Implement the Ladder (Arcade) progression UI.

## Phase 7: AI & Content Expansion
- [x] Create a Finite State Machine for AI (States: Approach, Retreat, Attack, Wait).
- [x] Implement Character 2-10 data structures in PROGMEM (Done in Phase 6).
- [x] Add AI difficulty scaling (increased aggression and faster reactions for higher ladder stages).
- [x] Refine Win and Game Over sequences.

## Phase 8: Polish & Optimization
- [ ] Optimize Bone rendering (unroll loops where possible).
- [ ] Add Sound Effects using `ArduboyPlaytune` or simple `beep`.
- [ ] Compress PROGMEM data if approaching the 32KB limit.
- [ ] Final bug hunt: Check for edge cases in corner collisions and parry timing.

## Phase 9: CLI & Build Automation
- [x] Create a `Makefile` for `arduino-cli`.
- [x] Document CLI build process in `GEMINI.md`.
- [x] Generate `.hex` file via command line.

## Misc
- [ ] Fix Characters sometimes pop across screen or disappear and re-appear on the other size
- [ ] Fix Characters don't display properly if not all on screen
- [ ] Add kick back for hits so the characters are a little further apart after a strike
- [ ] Stop spamming of attacks resulting in quick victories
- [ ] Fix characters walking through each other, they should have some volume in the world, you can jump over one if you want, then auto turn around.
- [ ] Add a little bit of dynamic movement to the legs like the upper body and arms
- [ ] You should take a little bit of damage from a block, about 1/5 of what the hit would have done, add a bit of knock back to both characters
- [ ] On the character ladder, when starting show the last two characters names crossed out, the character you'll be fighting next with the > and the one after that
- [ ] Pressing away from the other character should only block if they are punching or kicking at your from a short distance that could strike you.

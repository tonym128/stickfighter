# StickFighter Roster

StickFighter features 10 unique characters. Each character uses the same base bone structure but has different `Bone Lengths`, `Speeds`, and `Faces`.

## Character Roster
1.  **Zenith:** Balanced speed and range.
2.  **Cinder:** High speed, smaller hitbox.
3.  **Goliath:** Slow, long range limbs.
4.  **Volt:** Long legs for reaching kicks.
5.  **Kage:** Compact frame for close-range combat.
6.  **Siren:** Balanced frame.
7.  **Drift:** Light-weight feel.
8.  **Tusk:** Heavy frame with long reach.
9.  **Jade:** Fast and nimble.
10. **Echo:** Standardized frame.

## Universal Special Move
Every character in the roster can perform the **Fireball**.
- **Input:** ↓ → + Punch (A)
- **Effect:** Launches a projectile at shoulder height.
- **Strategic Use:** Excellent for controlling space and punishing retreats.

## Character Data (Technical)
Character properties are stored in `PROGMEM` within the `roster` array in `game_data.h`. 
- **Lengths:** Defines the visual size and reach of each bone.
- **Walk Speed:** Defines the horizontal movement velocity.
- **Face:** Defines the head shape and facial features rendered by `Engine::drawFace`.

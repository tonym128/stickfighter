# StickFighter Gameplay & Logic

## Core Loop
1.  **Input:** Poll buttons and update 60-frame input buffer.
2.  **Physics:** Apply gravity, friction, and movement.
3.  **Collision:** Check bone-to-bone intersections for hits (distance squared < 25).
4.  **State Update:** Trigger animations, deduct health, or update special meter.
5.  **Render:** Draw scaled background, stick figures, projectiles, and UI.

## Movement & Controls
- **Up Arrow:** Jump.
- **Down Arrow:** Duck (lowers character, changes attack hitbox).
- **Left/Right Arrow:** Walk.
- **Back (Away from Enemy):** Block. Stops movement, reduces damage by 80%.
- **Z (Punch / A):** Fast, shorter range (8 damage).
- **X (Kick / B):** Slower, longer range (12 damage).
- **R:** Restart (SDL only).
- **Q:** Quit (SDL only).

## Combat Mechanics

### 1. Special Moves
All characters share the **Fireball** special move.
- **Combo:** ↓ → + A (Down, Forward + Punch).
- **Cost:** 50% Special Meter.
- **Damage:** 25 damage on hit.
- **Visual:** Launches a projectile across the screen.

### 2. Parry System
- Triggered by pressing **Forward** just before being hit (8-frame window).
- **Reward:** Attacker is stunned for 40 frames, defender gains 25% Special Meter.

### 3. Blocking
- Hold Back while being attacked.
- Reduces health loss significantly (1/5th damage) but grants the attacker Special Meter.

### 4. Special Power Bar
- Fills by dealing damage, taking damage, or successful parries.
- Maxes out at 100%.
- Required for special moves (50% cost).

### 5. Ladder Mode (Arcade)
- Fight through the roster of 10 characters.
- Difficulty ramps up via AI (increased aggression and faster reactions).
- Win 2 rounds to progress to the next stage.

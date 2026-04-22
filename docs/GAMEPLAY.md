# StickFighter Gameplay & Logic

## Core Loop
1.  **Input:** Poll buttons and update input buffer.
2.  **Physics:** Apply gravity and movement.
3.  **Collision:** Check bone-to-bone intersections for hits.
4.  **State Update:** Trigger animations, deduct health, or update meters.
5.  **Render:** Draw scaled background and bone-based characters.

## Movement & Controls
- **Up:** Jump (fixed height or variable based on hold).
- **Down:** Duck (lowers hurtbox, changes available attacks).
- **Left/Right:** Walk.
- **Back (Away from Enemy):** Block. Stops movement, reduces damage/chip damage.
- **A Button:** Punch. Fast, short range.
- **B Button:** Kick. Slower, longer range.

## Combat Mechanics

### 1. Parry System
- Triggered by pressing **Forward** or **Neutral + Block** at the exact frame of impact.
- **Reward:** Zero chip damage, frame advantage (enemy is briefly stunned), and a massive boost to the Special Bar.

### 2. Blocking
- Hold Back while being attacked.
- Stops the player from moving backward further (cornered logic).
- Reduces health loss significantly but consumes a small amount of "Guard" or builds "Special" for the attacker.

### 3. Combo System
- Inputs are tracked in a 10-frame window.
- **Example:** `Down -> Right -> A` might trigger a "Fireball" or "Dash Punch".
- Cancel logic: A "Punch" can be cancelled into a "Kick" or "Special" if timed correctly.

### 4. Special Power Bar
- Fills by: Dealing damage, taking damage, or successful parries.
- **Levels:** 1-3 bars.
- **Usage:** "Super" moves or "EX" versions of normal specials.

### 5. Ladder Mode (Arcade)
- 10 fights total.
- Difficulty ramps up via AI (Faster reaction times, better combo execution).
- Final boss is a non-playable "Titan" character.

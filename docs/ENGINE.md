# StickFighter Engine: Bones & Scaling

## 1. Bone System
Characters are "Skeletal Stick Figures" built from a hierarchy of `Bone` structs.
- **Root Bones:** Torso and Legs.
- **Hip Offset:** Character root is automatically adjusted based on leg length to ensure all characters share the same ground plane.
- **Face Rendering:** Custom geometric primitive rendering for eyes, hair, and head shapes.

## 2. Animation Logic
- **Pose System:** 12 base poses (Idle, Walk, Attack, etc.) stored as arrays of angles.
- **Smoothing:** Linear interpolation between the current skeleton state and the target pose.
- **Special Effects:** 
  - **Breathing:** Periodic sine-wave offsets applied to torso and limb angles in the IDLE state.
  - **Walk Bob:** Sinusoidal vertical displacement during the WALK state.

## 3. Rendering & Camera
- **Dynamic Zoom:** Camera automatically scales based on the distance between players (clamped between 60% and 160%).
- **Shadows:** Elliptical shadows rendered dynamically at `GROUND_Y`.
- **Mirroring:** The engine supports horizontal bitmap mirroring for the main menu UI.

## 4. SDL Emulator (Development Tool)
- **Multi-Window Support:** The developer tools (Animation Editor) utilize separate SDL windows for the preview and UI controls.
- **Input Emulation:** Keyboard keys mapped 1:1 to Arduboy buttons, including support for mouse interaction in the editor.

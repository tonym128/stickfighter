# StickFighter Engine: Bones & Scaling

## 1. Bone System
Characters are not bitmaps; they are "Skeletal Stick Figures."
- **Structure:** A hierarchy of `Bone` structs.
  ```cpp
  struct Bone {
    int8_t length;
    int16_t angle; // Fixed-point or 0-255 for rotation
    int8_t parentIndex;
    // Calculated global position (X, Y)
  };
  ```
- **Animation:** Stored as "Keyframes" containing angles for each bone. Interpolate between angles for smooth 60fps movement.
- **Collision:** Collision is calculated by checking the distance between line segments (bones). A "Hit" occurs if an attacking bone segment comes within a certain radius of a hurtbox bone segment.

## 2. Fast Scaling Logic
To achieve "Zooming," we apply a `Scale` factor to all coordinates:
- **Local to Screen:** `ScreenX = (LocalX * Zoom) / 100 + OffsetX`.
- Use bit-shifting or fixed-point arithmetic instead of floating-point for speed.
- **Shapes:** Use Arduboy's `drawLine` and `drawCircle` for bones and joints. Scaling these is as simple as multiplying the endpoints by the zoom factor.

## 3. Dynamic Camera & Zoom
The camera calculates the distance between two fighters.
- **Far:** Zoom out to show the whole stage. Characters appear smaller.
- **Close:** Zoom in on the action. Characters appear larger, showing more "detail" (clearer joints).
- **Stage Limits:** Camera stops at the edges of the background.

## 4. Background Rendering
- **Grid/Simple Lines:** To maintain high performance while zooming, backgrounds will be composed of geometric primitives (e.g., a floor line, some distant pillars, a sun/moon circle).
- **Parallax:** Distant layers move slower than the foreground to give an illusion of depth.

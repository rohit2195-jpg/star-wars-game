# Camera System

## Overview

The camera follows Roan through the level. It is a 2D orthographic camera — it controls the offset applied to all world-space draw calls. There is no 3D projection.

The camera lives in `src/level/Camera.h / Camera.cpp` and is owned by `GameplayScene`.

---

## Camera Data

```cpp
class Camera {
public:
    SDL_FPoint  position;       // top-left corner of the viewport in world space
    int         viewWidth;      // viewport width in pixels (= window width)
    int         viewHeight;     // viewport height in pixels (= window height)
    float       zoom;           // 1.0 = native; 2.0 = 2× zoom in
    Entity*     target;         // entity the camera follows (Roan)

    float       lerpSpeed;      // how fast camera catches up (0.0–1.0; default: 0.08)
    float       lookAheadX;     // horizontal lookahead offset (pixels; default: 80)
    float       lookAheadY;     // vertical lookahead offset (pixels; default: 0)
    SDL_FRect   bounds;         // level bounds — camera won't scroll past these

    void update(float dt);
    SDL_FPoint worldToScreen(SDL_FPoint worldPos);
    SDL_FPoint screenToWorld(SDL_FPoint screenPos);
};
```

---

## Follow Behavior

### Horizontal Follow (Lerp)

Each frame, the camera's target X position is:
```
targetX = roan.x - (viewWidth / 2) + lookAheadX * roan.facingDirection
```

Where `facingDirection` is +1 (right) or -1 (left). The lookahead biases the camera forward so the player can see more of what's ahead.

The camera **lerps** toward this target each frame:
```
camera.x += (targetX - camera.x) * lerpSpeed
```

`lerpSpeed = 0.08` means the camera covers 8% of the remaining distance each frame. This gives smooth, organic following without feeling laggy.

### Vertical Follow

Vertical following uses a **dead zone**: a vertical band of ~4 tiles in the center of the screen where vertical camera movement is suppressed. The camera only moves vertically when Roan leaves this band.

```
deadZoneTop    = camera.y + viewHeight/2 - (4 * TILE_SIZE)
deadZoneBottom = camera.y + viewHeight/2 + (4 * TILE_SIZE)

if roan.y < deadZoneTop:
    targetY = roan.y - viewHeight/2 + (4 * TILE_SIZE)  // scroll up
elif roan.y > deadZoneBottom:
    targetY = roan.y - viewHeight/2 - (4 * TILE_SIZE)  // scroll down
else:
    targetY = camera.y  // no vertical change
```

Vertical also lerps but slightly slower (`lerpSpeedY = 0.05`) since vertical movement is less frequent and snappy vertical jumps would be disorienting.

### Level Bounds Clamping

After computing the lerp target, clamp to level bounds:
```
camera.x = clamp(camera.x, bounds.x, bounds.x + bounds.w - viewWidth)
camera.y = clamp(camera.y, bounds.y, bounds.y + bounds.h - viewHeight)
```

The level bounds are read from the Tiled map dimensions on load.

---

## World-to-Screen Transform

All world-space positions are converted to screen positions before drawing:

```cpp
SDL_FPoint Camera::worldToScreen(SDL_FPoint worldPos) {
    return {
        (worldPos.x - position.x) * zoom,
        (worldPos.y - position.y) * zoom
    };
}
```

The `RenderSystem` calls `worldToScreen()` on every entity's transform before passing the position to SDL_RenderCopyF.

---

## Zoom

Default zoom is `1.0` (native pixel size). Increasing zoom makes the world appear larger (zooms in).

At `zoom = 2.0`, a 16×16 tile renders as 32×32 on screen — effectively doubling the game's pixel art scale. The recommended zoom for a 1280×720 window with 16×16 tiles is **2.0** (tiles appear as 32×32 on screen, giving a comfortable view of ~40 tiles wide × ~22 tiles tall).

This can be changed in the `Game` initialization or made user-adjustable later.

---

## Screenshake

Screenshake is applied additively on top of the normal camera position. It does not affect the lerp target — it's a separate transient offset.

```cpp
void Camera::shake(float intensity, float durationSeconds) {
    shakeIntensity = intensity;
    shakeDuration  = durationSeconds;
    shakeTimer     = 0;
}

// In Camera::update():
if (shakeTimer < shakeDuration) {
    float t = shakeTimer / shakeDuration;
    float mag = shakeIntensity * (1.0f - t);  // fade out
    shakeOffset.x = randomFloat(-mag, mag);
    shakeOffset.y = randomFloat(-mag, mag);
    shakeTimer += dt;
} else {
    shakeOffset = {0, 0};
}
```

Applied draw position = `worldToScreen(pos) + shakeOffset`

**Shake triggers:**
| Event | Intensity | Duration |
|---|---|---|
| Light hit on Roan | 2.0 | 0.15s |
| Heavy hit on Roan | 5.0 | 0.3s |
| Boss slam/AoE | 8.0 | 0.5s |
| Explosion | 4.0 | 0.25s |
| Chamber collapse (Ch.5) | 12.0 | 1.0s |

---

## Cutscene Camera (Later Milestone)

For boss intros and chapter transitions, a **scripted camera** mode overrides the follow behavior. In this mode:
- The camera moves along a predefined path (list of target positions + durations)
- Lerp speed increases to `0.15` for more cinematic movement
- Player input is disabled
- Return to follow mode on path completion

The cutscene camera is not needed for Milestones 0–3. Implement when dialog/cutscene system is built.

---

## Parallax Rendering

The camera's `position` is used to calculate parallax layer offsets:

```cpp
// For each parallax layer:
float parallaxOffsetX = camera.position.x * layer.scrollFactor;
float parallaxOffsetY = camera.position.y * layer.scrollFactor;
// Render layer tiles starting at (-parallaxOffsetX, -parallaxOffsetY)
```

Layers with `scrollFactor < 1.0` appear to move slower than the foreground, creating depth.

#pragma once
#include <SDL2/SDL.h>

class Entity;

class Camera {
public:
    float  x          = 0.0f;
    float  y          = 0.0f;
    float  viewW      = 1280.0f;
    float  viewH      = 720.0f;
    float  zoom       = 2.0f;
    float  lerpSpeed  = 5.0f;
    float  lookaheadX = 80.0f;  // world-pixel offset in facing direction
    Entity* target    = nullptr;

    // w=0 disables clamping
    SDL_FRect bounds = {0.0f, 0.0f, 0.0f, 0.0f};

    // Screenshake state
    float shakeIntensity = 0.0f;
    float shakeDuration  = 0.0f;
    float shakeTimer     = 0.0f;
    float shakeOffsetX   = 0.0f;
    float shakeOffsetY   = 0.0f;

    void       update(float dt);
    void       shake(float intensity, float duration);
    SDL_FPoint worldToScreen(float wx, float wy) const;
    SDL_FPoint screenToWorld(float sx, float sy) const;
};

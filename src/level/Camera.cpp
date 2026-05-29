#include "level/Camera.h"
#include "entity/Entity.h"
#include <algorithm>
#include <cstdlib>

void Camera::update(float dt) {
    if (!target) return;

    float worldViewW = viewW / zoom;
    float worldViewH = viewH / zoom;
    float ahead      = target->transform.facingRight ? lookaheadX : -lookaheadX;

    float targetX = target->transform.x - worldViewW * 0.5f + ahead;
    float targetY = target->transform.y - worldViewH * 0.5f;

    x += (targetX - x) * lerpSpeed * dt;
    y += (targetY - y) * lerpSpeed * dt;

    if (bounds.w > 0.0f) {
        x = std::clamp(x, bounds.x, bounds.x + bounds.w - worldViewW);
        y = std::clamp(y, bounds.y, bounds.y + bounds.h - worldViewH);
    }

    if (shakeTimer > 0.0f) {
        shakeTimer -= dt;
        float t   = shakeTimer / shakeDuration;
        float amp = shakeIntensity * t;
        shakeOffsetX = static_cast<float>(rand() % 201 - 100) / 100.0f * amp;
        shakeOffsetY = static_cast<float>(rand() % 201 - 100) / 100.0f * amp;
    } else {
        shakeOffsetX = shakeOffsetY = 0.0f;
    }
}

void Camera::shake(float intensity, float duration) {
    shakeIntensity = intensity;
    shakeDuration  = duration;
    shakeTimer     = duration;
}

SDL_FPoint Camera::worldToScreen(float wx, float wy) const {
    return {
        (wx - x) * zoom + shakeOffsetX,
        (wy - y) * zoom + shakeOffsetY
    };
}

SDL_FPoint Camera::screenToWorld(float sx, float sy) const {
    return {
        (sx - shakeOffsetX) / zoom + x,
        (sy - shakeOffsetY) / zoom + y
    };
}

#pragma once
#include <SDL2/SDL.h>

class Time {
public:
    void tick();

    float dt()  const { return deltaTime; }
    float fps() const { return 1.0f / (deltaTime > 0.0f ? deltaTime : 1.0f); }

    // Call once per frame after tick() to sleep until the next frame start.
    void capFPS(int targetFPS);

private:
    Uint64 lastTicks  = 0;
    Uint64 frameStart = 0;
    float  deltaTime  = 0.0f;
};

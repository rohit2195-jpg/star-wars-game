#include "Time.h"
#include <algorithm>

void Time::tick() {
    frameStart = SDL_GetTicks64();

    if (lastTicks == 0) {
        // First frame — no meaningful delta yet.
        lastTicks = frameStart;
        deltaTime = 0.0f;
        return;
    }

    Uint64 elapsed = frameStart - lastTicks;
    lastTicks = frameStart;

    // Clamp to 250ms max (avoids spiral-of-death if debugger pauses the process).
    elapsed   = std::min(elapsed, (Uint64)250);
    deltaTime = static_cast<float>(elapsed) / 1000.0f;
}

void Time::capFPS(int targetFPS) {
    Uint64 frameMs     = 1000 / static_cast<Uint64>(targetFPS);
    Uint64 elapsed     = SDL_GetTicks64() - frameStart;
    if (elapsed < frameMs) {
        SDL_Delay(static_cast<Uint32>(frameMs - elapsed));
    }
}

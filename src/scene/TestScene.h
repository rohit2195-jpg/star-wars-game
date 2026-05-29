#pragma once
#include "Scene.h"
#include <SDL2/SDL.h>

// Milestone 0 smoke-test: draws a colored rectangle that moves left/right
// with A/D, confirms the game loop, input, and renderer work.
class TestScene : public Scene {
public:
    void onEnter() override;
    void update(float dt, Input& input) override;
    void render(SDL_Renderer* renderer) override;

private:
    float x = 600.0f;
    float y = 340.0f;
};

#include "TestScene.h"
#include "core/Input.h"
#include <SDL2/SDL.h>

static constexpr float SPEED = 300.0f; // pixels per second

void TestScene::onEnter() {
    SDL_Log("TestScene: entered. Use A/D to move the box. Escape to quit.");
}

void TestScene::update(float dt, Input& input) {
    if (input.isHeld(SDL_SCANCODE_A)) x -= SPEED * dt;
    if (input.isHeld(SDL_SCANCODE_D)) x += SPEED * dt;
    if (input.isHeld(SDL_SCANCODE_W)) y -= SPEED * dt;
    if (input.isHeld(SDL_SCANCODE_S)) y += SPEED * dt;
}

void TestScene::render(SDL_Renderer* renderer) {
    // Dark background
    SDL_SetRenderDrawColor(renderer, 15, 15, 30, 255);
    SDL_RenderClear(renderer);

    // A green rectangle representing Roan as a placeholder
    SDL_FRect box = { x - 16.0f, y - 24.0f, 32.0f, 48.0f };
    SDL_SetRenderDrawColor(renderer, 80, 200, 80, 255);
    SDL_RenderFillRectF(renderer, &box);

    // A simple "floor"
    SDL_Rect floor = { 0, 600, 1280, 120 };
    SDL_SetRenderDrawColor(renderer, 60, 40, 20, 255);
    SDL_RenderFillRect(renderer, &floor);
}

#include "scene/GameplayScene.h"
#include "systems/CollisionSystem.h"
#include "entity/components/SpriteComponent.h"

void GameplayScene::onEnter() {
    player = std::make_unique<Player>();

    // Load sprite sheet (PNG via SDL2_image, cached by AssetManager)
    SDL_Texture* tex = assets.getTexture("assets/sprites/roan_sheet.png");
    if (tex && player->sprite) {
        player->sprite->texture = tex;
    }

    camera.target = player.get();
    camera.zoom   = 2.0f;

    // Snap camera to player immediately (no lerp on first frame)
    float worldViewW = camera.viewW / camera.zoom;
    float worldViewH = camera.viewH / camera.zoom;
    camera.x = player->transform.x - worldViewW * 0.5f;
    camera.y = player->transform.y - worldViewH * 0.5f;
}

void GameplayScene::update(float dt, Input& input) {
    if (input.isPressed(SDL_SCANCODE_F1)) debugDraw = !debugDraw;

    bool wasGrounded = player->physics->grounded;
    player->update(dt, input);
    CollisionSystem::resolveFloor(*player, FLOOR_Y);
    player->postCollide(wasGrounded, dt);

    camera.update(dt);
}

void GameplayScene::render(SDL_Renderer* renderer) {
    // Sky background
    SDL_SetRenderDrawColor(renderer, 10, 10, 25, 255);
    SDL_RenderClear(renderer);

    // Floor
    SDL_FPoint floorTL = camera.worldToScreen(-1000.0f, FLOOR_Y);
    SDL_FRect  floor   = {
        floorTL.x,
        floorTL.y,
        11000.0f * camera.zoom,
         200.0f * camera.zoom
    };
    SDL_SetRenderDrawColor(renderer, 50, 35, 20, 255);
    SDL_RenderFillRectF(renderer, &floor);

    // Update sprite camera transform
    if (player->sprite) {
        player->sprite->camX = camera.x;
        player->sprite->camY = camera.y;
        player->sprite->zoom = camera.zoom;
    }

    player->render(renderer);

    // Debug: collider AABB + floor line
    if (debugDraw) {
        CollisionSystem::debugRender(renderer, player.get(),
                                      camera.x, camera.y, camera.zoom);

        SDL_FPoint fl = camera.worldToScreen(-1000.0f, FLOOR_Y);
        SDL_FPoint fr = camera.worldToScreen(10000.0f, FLOOR_Y);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 220);
        SDL_RenderDrawLineF(renderer, fl.x, fl.y, fr.x, fr.y);
    }
}

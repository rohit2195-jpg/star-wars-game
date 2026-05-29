#pragma once
#include <SDL2/SDL.h>
#include "entity/Component.h"

class SpriteComponent : public Component {
public:
    SDL_Texture* texture     = nullptr;
    SDL_Rect     srcRect     = {0, 0, 48, 48};
    int          drawOffsetX = 0;
    int          drawOffsetY = 0;
    int          drawWidth   = 0;   // 0 = use srcRect.w
    int          drawHeight  = 0;   // 0 = use srcRect.h

    // Set by GameplayScene each frame before render()
    float camX = 0.0f;
    float camY = 0.0f;
    float zoom = 1.0f;

    void render(SDL_Renderer* renderer) override;
};

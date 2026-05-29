#pragma once
#include <functional>
#include <SDL2/SDL.h>
#include "entity/Component.h"

class ColliderComponent : public Component {
public:
    float offsetX  = 0.0f;
    float offsetY  = 0.0f;
    float width    = 32.0f;
    float height   = 48.0f;
    bool  isTrigger = false;

    std::function<void(Entity*)> onCollide;

    SDL_FRect getRect() const;
};

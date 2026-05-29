#pragma once
#include <SDL2/SDL.h>
#include "core/Input.h"

class Scene {
public:
    virtual ~Scene() = default;

    // Called when this scene becomes the active top-of-stack scene.
    virtual void onEnter() {}

    // Called just before this scene is popped or swapped out.
    virtual void onExit() {}

    virtual void update(float dt, Input& input) = 0;
    virtual void render(SDL_Renderer* renderer)  = 0;
};

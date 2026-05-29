#pragma once
#include <memory>
#include "scene/Scene.h"
#include "level/Camera.h"
#include "entities/Player.h"

class GameplayScene : public Scene {
public:
    void onEnter() override;
    void update(float dt, Input& input) override;
    void render(SDL_Renderer* renderer) override;

private:
    static constexpr float FLOOR_Y = 600.0f;

    std::unique_ptr<Player> player;
    Camera                  camera;
    bool                    debugDraw = false;
};

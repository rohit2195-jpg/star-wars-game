#pragma once
#include <memory>
#include "scene/Scene.h"
#include "level/Camera.h"
#include "entities/Player.h"
#include "core/AssetManager.h"

class GameplayScene : public Scene {
public:
    explicit GameplayScene(AssetManager& assets) : assets(assets) {}

    void onEnter() override;
    void update(float dt, Input& input) override;
    void render(SDL_Renderer* renderer) override;

private:
    static constexpr float FLOOR_Y = 600.0f;

    AssetManager&           assets;
    std::unique_ptr<Player> player;
    Camera                  camera;
    bool                    debugDraw = false;
};

#include "core/Game.h"
#include "scene/TestScene.h"
#include <memory>

int main(int /*argc*/, char* /*argv*/[]) {
    Game game;

    if (!game.init("Star Wars: The Holocron of Roan Novachez", 1280, 720)) {
        return 1;
    }

    game.getScenes().push(std::make_unique<TestScene>());

    game.run();

    return 0;
}

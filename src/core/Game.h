#pragma once
#include <SDL2/SDL.h>
#include <string>
#include "core/Time.h"
#include "core/Input.h"
#include "core/AssetManager.h"
#include "scene/SceneManager.h"

class Game {
public:
    Game();
    ~Game();

    bool init(const std::string& title, int width, int height);
    void run();
    void quit() { running = false; }

    SDL_Renderer*  getRenderer() { return renderer; }
    AssetManager&  getAssets()   { return assets;   }
    SceneManager&  getScenes()   { return scenes;   }
    Input&         getInput()    { return input;    }

private:
    void processEvents();

    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool          running  = false;

    Time          time;
    Input         input;
    AssetManager  assets;
    SceneManager  scenes;
};

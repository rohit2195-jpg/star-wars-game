#include "AssetManager.h"
#include <SDL2/SDL.h>

AssetManager::~AssetManager() {
    freeAll();
}

void AssetManager::init(SDL_Renderer* r) {
    renderer = r;

#ifdef HAS_SDL2_IMAGE
    int flags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(flags) & flags) != flags) {
        SDL_Log("SDL2_image init warning: %s", IMG_GetError());
    }
#endif
}

SDL_Texture* AssetManager::getTexture(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) return it->second;

    SDL_Surface* surface = nullptr;

#ifdef HAS_SDL2_IMAGE
    surface = IMG_Load(path.c_str());
#else
    surface = SDL_LoadBMP(path.c_str());
#endif

    if (!surface) {
        SDL_Log("AssetManager: failed to load '%s': %s", path.c_str(), SDL_GetError());
        return nullptr;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!tex) {
        SDL_Log("AssetManager: failed to create texture from '%s': %s", path.c_str(), SDL_GetError());
        return nullptr;
    }

    textures[path] = tex;
    return tex;
}

void AssetManager::freeAll() {
    for (auto& [path, tex] : textures) {
        SDL_DestroyTexture(tex);
    }
    textures.clear();

#ifdef HAS_SDL2_IMAGE
    IMG_Quit();
#endif
}

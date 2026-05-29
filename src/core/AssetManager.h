#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>

#ifdef HAS_SDL2_IMAGE
#include <SDL2/SDL_image.h>
#endif

class AssetManager {
public:
    AssetManager() = default;
    ~AssetManager();

    // Must be called before any load* calls.
    void init(SDL_Renderer* renderer);

    // Returns a cached texture, loading it on first call.
    // Supports PNG (with SDL2_image) or BMP (fallback).
    // Returns nullptr on failure.
    SDL_Texture* getTexture(const std::string& path);

    void freeAll();

private:
    SDL_Renderer* renderer = nullptr;
    std::unordered_map<std::string, SDL_Texture*> textures;
};

#pragma once
#include <SDL2/SDL.h>
#include <vector>

class Input {
public:
    // Call once per frame before processing game logic.
    void update();

    // True every frame the key is held down.
    bool isHeld(SDL_Scancode key) const;

    // True only on the first frame the key is pressed.
    bool isPressed(SDL_Scancode key) const;

    // True only on the frame the key is released.
    bool isReleased(SDL_Scancode key) const;

private:
    std::vector<Uint8> current;
    std::vector<Uint8> previous;
    int                numKeys = 0;
};

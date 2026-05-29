#include "Input.h"

void Input::update() {
    const Uint8* state = SDL_GetKeyboardState(&numKeys);

    previous = current;
    current.assign(state, state + numKeys);
}

bool Input::isHeld(SDL_Scancode key) const {
    if (key >= numKeys) return false;
    return current[key] != 0;
}

bool Input::isPressed(SDL_Scancode key) const {
    if (key >= numKeys) return false;
    return current[key] != 0 && (previous.empty() || previous[key] == 0);
}

bool Input::isReleased(SDL_Scancode key) const {
    if (key >= numKeys) return false;
    return current[key] == 0 && !previous.empty() && previous[key] != 0;
}

#include "entity/components/ColliderComponent.h"
#include "entity/Entity.h"

SDL_FRect ColliderComponent::getRect() const {
    return {
        owner->transform.x + offsetX,
        owner->transform.y + offsetY,
        width,
        height
    };
}

#include "entity/components/SpriteComponent.h"
#include "entity/Entity.h"

void SpriteComponent::render(SDL_Renderer* renderer) {
    float w = (float)(drawWidth  > 0 ? drawWidth  : srcRect.w);
    float h = (float)(drawHeight > 0 ? drawHeight : srcRect.h);

    SDL_FRect dest = {
        (owner->transform.x + (float)drawOffsetX - camX) * zoom,
        (owner->transform.y + (float)drawOffsetY - camY) * zoom,
        w * zoom,
        h * zoom
    };

    SDL_RendererFlip flip = owner->transform.facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    if (texture && srcRect.w > 0 && srcRect.h > 0) {
        SDL_RenderCopyExF(renderer, texture, &srcRect, &dest, 0.0, nullptr, flip);
    } else {
        // Placeholder: colored rect (green for player, replace when real sprites arrive)
        SDL_SetRenderDrawColor(renderer, 80, 200, 80, 255);
        SDL_RenderFillRectF(renderer, &dest);
    }
}

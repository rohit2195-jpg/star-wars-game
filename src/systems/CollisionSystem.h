#pragma once
#include <SDL2/SDL.h>
#include "entity/Entity.h"
#include "entity/components/ColliderComponent.h"
#include "entity/components/PhysicsComponent.h"

class CollisionSystem {
public:
    // Snaps entity above floorY and sets grounded=true if touching floor,
    // otherwise sets grounded=false.
    static void resolveFloor(Entity& entity, float floorY) {
        auto* col  = entity.getComponent<ColliderComponent>();
        auto* phys = entity.getComponent<PhysicsComponent>();
        if (!col || !phys) return;

        SDL_FRect rect   = col->getRect();
        float     bottom = rect.y + rect.h;

        if (bottom >= floorY && phys->velY >= 0.0f) {
            entity.transform.y = floorY - col->height - col->offsetY;
            phys->velY         = 0.0f;
            phys->grounded     = true;
        } else {
            phys->grounded = false;
        }
    }

    // Draws a green AABB wireframe for the entity's collider (debug only).
    static void debugRender(SDL_Renderer* renderer, Entity* entity,
                             float camX, float camY, float zoom) {
        auto* col = entity->getComponent<ColliderComponent>();
        if (!col) return;

        SDL_FRect wr = col->getRect();
        SDL_FRect sr = {
            (wr.x - camX) * zoom,
            (wr.y - camY) * zoom,
            wr.w * zoom,
            wr.h * zoom
        };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 220);
        SDL_RenderDrawRectF(renderer, &sr);
    }
};

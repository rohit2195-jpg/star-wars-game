#include "entity/components/PhysicsComponent.h"
#include "entity/Entity.h"

void PhysicsComponent::update(float dt) {
    if (applyGravity && !grounded) {
        velY += gravity * dt;
        if (velY > maxFallSpeed) velY = maxFallSpeed;
    }
    owner->transform.x += velX * dt;
    owner->transform.y += velY * dt;
}

#pragma once
#include "entity/Component.h"

class PhysicsComponent : public Component {
public:
    float velX         = 0.0f;
    float velY         = 0.0f;
    float gravity      = 1800.0f;
    float maxFallSpeed = 800.0f;
    bool  grounded     = false;
    bool  applyGravity = true;

    void update(float dt) override;
};

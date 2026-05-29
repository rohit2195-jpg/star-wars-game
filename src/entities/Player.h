#pragma once
#include "entity/Entity.h"
#include "core/Input.h"

class SpriteComponent;
class AnimatorComponent;
class PhysicsComponent;
class ColliderComponent;

class Player : public Entity {
public:
    static constexpr float RUN_SPEED       = 280.0f;
    static constexpr float WALK_SPEED      = 140.0f;
    static constexpr float JUMP_VELOCITY   = 620.0f;
    static constexpr float MIN_JUMP_VEL    = 200.0f;
    static constexpr float ROLL_SPEED      = 420.0f;
    static constexpr float ROLL_DURATION   = 0.20f;
    static constexpr float ROLL_COOLDOWN   = 0.33f;
    static constexpr float GROUND_FRICTION = 1800.0f;
    static constexpr float AIR_CONTROL     = 0.7f;
    static constexpr float COYOTE_TIME     = 0.10f;
    static constexpr float JUMP_BUFFER_DUR = 0.13f;

    Player();

    using Entity::update;
    void update(float dt, Input& input);

    // Called by GameplayScene AFTER CollisionSystem::resolveFloor each frame.
    void postCollide(bool wasGrounded, float dt);

    // Raw component pointers for GameplayScene to access (e.g. set camera offset on sprite).
    SpriteComponent*   sprite   = nullptr;
    AnimatorComponent* animator = nullptr;
    PhysicsComponent*  physics  = nullptr;
    ColliderComponent* collider = nullptr;

private:
    float coyoteTimer  = 0.0f;
    float jumpBuffer   = 0.0f;
    bool  jumpHeld     = false;
    bool  justLanded   = false;
    float rollTimer    = 0.0f;
    float rollCooldown = 0.0f;
    int   rollDir      = 1;

    void updateMovement(float dt, const Input& input);
    void updateJump(float dt, const Input& input);
    void updateRoll(float dt, const Input& input);
    void updateAnimation();
};

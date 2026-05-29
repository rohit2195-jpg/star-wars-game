#include "entities/Player.h"
#include "entity/components/SpriteComponent.h"
#include "entity/components/AnimatorComponent.h"
#include "entity/components/PhysicsComponent.h"
#include "entity/components/ColliderComponent.h"
#include <cmath>

Player::Player() {
    transform.x = 300.0f;
    transform.y = 552.0f; // feet at floor level (600) minus sprite height (48)

    sprite   = addComponent<SpriteComponent>();
    animator = addComponent<AnimatorComponent>();
    physics  = addComponent<PhysicsComponent>();
    collider = addComponent<ColliderComponent>();

    physics->gravity      = 1800.0f;
    physics->maxFallSpeed = 800.0f;

    // AABB: centered on transform, bottom at transform.y + 48
    collider->offsetX = -16.0f;
    collider->offsetY =   0.0f;
    collider->width   =  32.0f;
    collider->height  =  48.0f;

    // Sprite origin offset so feet align with transform position
    sprite->drawOffsetX = -24;
    sprite->drawOffsetY =   0;
    sprite->drawWidth   =  48;
    sprite->drawHeight  =  48;
    sprite->srcRect     = {0, 0, 48, 48};

    // Animation clips: sheetRow, frameCount, frameWidth, frameHeight, fps, loop
    animator->addClip("idle",      {0, 4, 48, 48,  6.0f, true });
    animator->addClip("walk",      {1, 6, 48, 48,  8.0f, true });
    animator->addClip("run",       {2, 8, 48, 48, 12.0f, true });
    animator->addClip("jump_rise", {3, 2, 48, 48,  8.0f, false});
    animator->addClip("jump_apex", {4, 1, 48, 48,  4.0f, false});
    animator->addClip("jump_fall", {5, 2, 48, 48,  8.0f, true });
    animator->addClip("jump_land", {6, 3, 48, 48, 12.0f, false});
    animator->addClip("roll",      {7, 8, 48, 48, 24.0f, false});

    animator->play("idle");
}

void Player::update(float dt, Input& input) {
    updateRoll(dt, input);
    updateJump(dt, input);
    updateMovement(dt, input);
    Entity::update(dt);      // runs physics (moves transform) and animator
    updateAnimation();
}

void Player::postCollide(bool wasGrounded, float dt) {
    bool nowGrounded = physics->grounded;

    // Coyote time: player walked off ledge without jumping
    if (wasGrounded && !nowGrounded && !jumpHeld) {
        coyoteTimer = COYOTE_TIME;
    } else if (!nowGrounded && coyoteTimer > 0.0f) {
        coyoteTimer -= dt;
        if (coyoteTimer < 0.0f) coyoteTimer = 0.0f;
    } else if (nowGrounded) {
        coyoteTimer = 0.0f;
    }

    // Landing detection
    if (!wasGrounded && nowGrounded) {
        justLanded = true;
    }

    // End landing state when jump_land animation finishes
    if (justLanded && animator->isFinished()) {
        justLanded = false;
    }
}

void Player::updateMovement(float dt, const Input& input) {
    if (rollTimer > 0.0f) {
        physics->velX = (float)rollDir * ROLL_SPEED;
        return;
    }

    bool left  = input.isHeld(SDL_SCANCODE_A);
    bool right = input.isHeld(SDL_SCANCODE_D);

    float accel = physics->grounded ? GROUND_FRICTION : GROUND_FRICTION * AIR_CONTROL;

    if (right && !left) {
        physics->velX += accel * dt;
        if (physics->velX >  RUN_SPEED) physics->velX =  RUN_SPEED;
        transform.facingRight = true;
    } else if (left && !right) {
        physics->velX -= accel * dt;
        if (physics->velX < -RUN_SPEED) physics->velX = -RUN_SPEED;
        transform.facingRight = false;
    } else {
        float friction = GROUND_FRICTION * dt;
        if (physics->velX > 0.0f) {
            physics->velX -= friction;
            if (physics->velX < 0.0f) physics->velX = 0.0f;
        } else if (physics->velX < 0.0f) {
            physics->velX += friction;
            if (physics->velX > 0.0f) physics->velX = 0.0f;
        }
    }
}

void Player::updateJump(float dt, const Input& input) {
    if (input.isPressed(SDL_SCANCODE_SPACE)) {
        jumpBuffer = JUMP_BUFFER_DUR;
    } else if (jumpBuffer > 0.0f) {
        jumpBuffer -= dt;
        if (jumpBuffer < 0.0f) jumpBuffer = 0.0f;
    }

    bool canJump = (physics->grounded || coyoteTimer > 0.0f) && jumpBuffer > 0.0f;
    if (canJump) {
        physics->velY  = -JUMP_VELOCITY;
        jumpHeld       = true;
        jumpBuffer     = 0.0f;
        coyoteTimer    = 0.0f;
    }

    // Variable height: releasing Space cuts jump arc short
    if (input.isReleased(SDL_SCANCODE_SPACE) && jumpHeld && physics->velY < -MIN_JUMP_VEL) {
        physics->velY = -MIN_JUMP_VEL;
        jumpHeld      = false;
    }
    if (physics->grounded) jumpHeld = false;
}

void Player::updateRoll(float dt, const Input& input) {
    if (rollTimer > 0.0f) {
        rollTimer -= dt;
        if (rollTimer <= 0.0f) {
            rollTimer    = 0.0f;
            rollCooldown = ROLL_COOLDOWN;
        }
        return;
    }
    if (rollCooldown > 0.0f) {
        rollCooldown -= dt;
        if (rollCooldown < 0.0f) rollCooldown = 0.0f;
    }

    if (input.isPressed(SDL_SCANCODE_LSHIFT) && physics->grounded && rollCooldown <= 0.0f) {
        rollTimer    = ROLL_DURATION;
        rollDir      = transform.facingRight ? 1 : -1;
        physics->velX = (float)rollDir * ROLL_SPEED;
    }
}

void Player::updateAnimation() {
    if (rollTimer > 0.0f)          { animator->play("roll");      return; }
    if (justLanded)                { animator->play("jump_land"); return; }

    if (!physics->grounded) {
        if      (physics->velY < -50.0f)                   animator->play("jump_rise");
        else if (std::abs(physics->velY) <= 50.0f)         animator->play("jump_apex");
        else                                                animator->play("jump_fall");
        return;
    }

    float absVel = std::abs(physics->velX);
    if      (absVel >= RUN_SPEED * 0.8f) animator->play("run");
    else if (absVel > 5.0f)             animator->play("walk");
    else                                animator->play("idle");
}

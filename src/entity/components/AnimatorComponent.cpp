#include "entity/components/AnimatorComponent.h"
#include "entity/components/SpriteComponent.h"
#include "entity/Entity.h"

void AnimatorComponent::addClip(const std::string& name, const AnimationClip& clip) {
    clips[name] = clip;
}

void AnimatorComponent::play(const std::string& name) {
    if (currentClip == name) return;
    currentClip  = name;
    currentFrame = 0;
    timer        = 0.0f;
    finished     = false;
}

bool AnimatorComponent::isFinished() const { return finished; }

void AnimatorComponent::update(float dt) {
    if (currentClip.empty()) return;
    auto it = clips.find(currentClip);
    if (it == clips.end()) return;

    if (!spriteCache) spriteCache = owner->getComponent<SpriteComponent>();
    if (!spriteCache) return;

    const AnimationClip& clip = it->second;
    timer += dt;
    float frameDuration = 1.0f / clip.fps;

    while (timer >= frameDuration) {
        timer -= frameDuration;
        currentFrame++;
        if (currentFrame >= clip.frameCount) {
            if (clip.loop) {
                currentFrame = 0;
            } else {
                currentFrame = clip.frameCount - 1;
                finished     = true;
            }
        }
    }

    spriteCache->srcRect = {
        currentFrame * clip.frameWidth,
        clip.sheetRow * clip.frameHeight,
        clip.frameWidth,
        clip.frameHeight
    };
}

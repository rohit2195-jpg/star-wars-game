#pragma once
#include <string>
#include <unordered_map>
#include "entity/Component.h"

struct AnimationClip {
    int   sheetRow    = 0;
    int   frameCount  = 1;
    int   frameWidth  = 48;
    int   frameHeight = 48;
    float fps         = 8.0f;
    bool  loop        = true;
};

class SpriteComponent;

class AnimatorComponent : public Component {
public:
    std::unordered_map<std::string, AnimationClip> clips;

    void addClip(const std::string& name, const AnimationClip& clip);
    void play(const std::string& name);
    bool isFinished() const;

    void update(float dt) override;

private:
    std::string    currentClip;
    int            currentFrame  = 0;
    float          timer         = 0.0f;
    bool           finished      = false;
    SpriteComponent* spriteCache = nullptr;
};

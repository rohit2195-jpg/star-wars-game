#pragma once
#include <memory>
#include <vector>
#include "Scene.h"
#include "core/Input.h"

class SceneManager {
public:
    // Push a new scene. Previous scene is paused (not destroyed) beneath it.
    void push(std::unique_ptr<Scene> scene);

    // Destroy the top scene. Previous scene resumes.
    void pop();

    // Destroy the top scene and push a new one (level transition).
    void swap(std::unique_ptr<Scene> scene);

    bool isEmpty() const { return stack.empty(); }

    void update(float dt, Input& input);
    void render(SDL_Renderer* renderer);

    // Deferred operations are applied at the start of the next frame
    // so push/pop during update() does not invalidate iterators.
    void applyPending();

private:
    std::vector<std::unique_ptr<Scene>> stack;

    // Pending operations ─ applied by applyPending() at the start of each frame.
    enum class OpType { Push, Pop, Swap };
    struct PendingOp {
        OpType                    type;
        std::unique_ptr<Scene>    scene; // valid for Push and Swap
    };
    std::vector<PendingOp> pending;
};

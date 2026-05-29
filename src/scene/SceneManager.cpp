#include "SceneManager.h"

void SceneManager::push(std::unique_ptr<Scene> scene) {
    pending.push_back({ OpType::Push, std::move(scene) });
}

void SceneManager::pop() {
    pending.push_back({ OpType::Pop, nullptr });
}

void SceneManager::swap(std::unique_ptr<Scene> scene) {
    pending.push_back({ OpType::Swap, std::move(scene) });
}

void SceneManager::applyPending() {
    for (auto& op : pending) {
        switch (op.type) {
        case OpType::Push:
            if (!stack.empty()) { /* top scene continues underneath */ }
            stack.push_back(std::move(op.scene));
            stack.back()->onEnter();
            break;

        case OpType::Pop:
            if (!stack.empty()) {
                stack.back()->onExit();
                stack.pop_back();
                if (!stack.empty()) stack.back()->onEnter();
            }
            break;

        case OpType::Swap:
            if (!stack.empty()) {
                stack.back()->onExit();
                stack.pop_back();
            }
            stack.push_back(std::move(op.scene));
            stack.back()->onEnter();
            break;
        }
    }
    pending.clear();
}

void SceneManager::update(float dt, Input& input) {
    if (!stack.empty()) {
        stack.back()->update(dt, input);
    }
}

void SceneManager::render(SDL_Renderer* renderer) {
    // Render the bottom-most scene first so overlays (dialogs, pause) draw on top.
    for (auto& scene : stack) {
        scene->render(renderer);
    }
}

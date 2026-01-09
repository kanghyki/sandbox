#include "engine/scene/SceneManager.h"

void SceneManager::AddScene(std::unique_ptr<IScene> scene) {
    if (!scene) {
        return;
    }
    scenes_.push_back(std::move(scene));
    if (scenes_.size() == 1) {
        active_index_ = 0;
        if (IScene* active = scenes_[0].get()) {
            active->OnEnter();
        }
    }
}

IScene* SceneManager::GetScene(size_t index) {
    if (index >= scenes_.size()) {
        return nullptr;
    }
    return scenes_[index].get();
}

const IScene* SceneManager::GetScene(size_t index) const {
    if (index >= scenes_.size()) {
        return nullptr;
    }
    return scenes_[index].get();
}

void SceneManager::SetActiveIndex(size_t index) {
    if (index >= scenes_.size() || index == active_index_) {
        return;
    }
    if (IScene* current = ActiveScene()) {
        current->OnExit();
    }
    active_index_ = index;
    if (IScene* next = ActiveScene()) {
        next->OnEnter();
    }
}

IScene* SceneManager::ActiveScene() { return GetScene(active_index_); }

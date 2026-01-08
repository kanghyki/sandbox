#include "engine/scene/SceneManager.h"

void SceneManager::AddScene(std::unique_ptr<IScene> scene) {
    if (!scene) {
        return;
    }
    scenes_.push_back(std::move(scene));
    if (scenes_.size() == 1) {
        active_index_ = 0;
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
    if (index < scenes_.size()) {
        active_index_ = index;
    }
}

IScene* SceneManager::ActiveScene() { return GetScene(active_index_); }

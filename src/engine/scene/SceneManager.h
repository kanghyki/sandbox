#pragma once

#include "engine/scene/IScene.h"

#include <memory>
#include <vector>

class SceneManager {
  public:
    void AddScene(std::unique_ptr<IScene> scene);

    size_t SceneCount() const { return scenes_.size(); }
    IScene* GetScene(size_t index);
    const IScene* GetScene(size_t index) const;

    void SetActiveIndex(size_t index);
    size_t ActiveIndex() const { return active_index_; }
    IScene* ActiveScene();

  private:
    std::vector<std::unique_ptr<IScene>> scenes_;
    size_t active_index_ = 0;
};

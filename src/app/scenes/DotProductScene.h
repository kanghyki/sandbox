#pragma once

#include "engine/scene/IScene.h"

class DotProductScene : public IScene {
  public:
    const char* Name() const override { return "Dot Product"; }
    void Reset() override;
    void Update(float dt) override;
    void Render(IRenderer& renderer) override;

  private:
    float time_ = 0.0f;
};

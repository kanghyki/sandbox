#pragma once

#include "engine/scene/IScene.h"

class Example2DScene : public IScene {
  public:
    const char* Name() const override { return "Example 2D"; }
    void Reset() override;
    void Update(const FrameContext& context) override;
    void Render(IRenderer& renderer) override;

  private:
    float time_ = 0.0f;
};

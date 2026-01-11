#pragma once

#include "engine/core/Color4f.h"
#include "engine/math/sgm/public/sgm.h"
#include "engine/scene/IScene.h"

#include <vector>

class CircleScene : public IScene {
  public:
    const char* Name() const override { return "CircleScene"; }
    void Reset() override;
    void Update(const FrameContext& context) override;
    void Render(IRenderer& renderer) override;
    void DrawSceneGui() override;
    void DrawInspectorGui() override;

  private:
    float time_ = 0.0f;

    float radius_ = 50.0f;
    float prevRadius_ = 0.0f;

    std::vector<sgm::vec2> circle_;

    Color4f color_{1.0f, 0.0f, 0.0f, 1.0f};
};

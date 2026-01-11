#pragma once

#include "engine/core/Color4f.h"
#include "engine/scene/IScene.h"
#include "sgm.h"

#include <vector>

class HeartScene : public IScene {
  public:
    const char* Name() const override { return "HeartScene"; }
    void OnEnter() override;
    void OnExit() override;
    void Reset() override;
    void Update(const FrameContext& context) override;
    void Render(IRenderer& renderer) override;
    void DrawSceneGui() override;
    void DrawInspectorGui() override;

  private:
    float time_ = 0.0f;
    bool isInitPosition_ = false;
    sgm::vec2 position_{0.0f};
    std::vector<sgm::vec2> hearts_;
    float scale_ = 5.0f;
    float amplitude_ = 5.0f;
    Color4f color_{1.0f, 1.0f, 1.0f, 1.0f};
};

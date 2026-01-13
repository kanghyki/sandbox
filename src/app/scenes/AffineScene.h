#pragma once

#include "engine/scene/IScene.h"
#include "mat3.h"
#include "vec3.h"

#include <array>

class AffineScene : public IScene {
  public:
    AffineScene();
    ~AffineScene();
    AffineScene(const AffineScene&) = delete;
    AffineScene& operator=(const AffineScene&) = delete;

    const char* Name() const override { return "AffineScene"; }
    void OnEnter() override;
    void OnExit() override;
    void Reset() override;
    void Update(const FrameContext& context) override;
    void Render(IRenderer& renderer) override;
    void DrawSceneGui() override;
    void DrawInspectorGui() override;

  private:
    float time_ = 0.0f;

    std::vector<sgm::vec3> box_;
    sgm::mat3 S;
    sgm::mat3 R;
    sgm::mat3 T;
    std::array<bool, 3> flags;
};

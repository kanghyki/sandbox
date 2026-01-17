#pragma once

#include "engine/scene/IScene.h"

class DotProjScene : public IScene {
  public:
    DotProjScene();
    ~DotProjScene();
    DotProjScene(const DotProjScene&) = delete;
    DotProjScene& operator=(const DotProjScene&) = delete;

    const char* Name() const override { return "DotProjScene"; }
    void OnEnter() override;
    void OnExit() override;
    void Reset() override;
    void Update(const FrameContext& context) override;
    void Render(IRenderer& renderer) override;
    void DrawSceneGui() override;
    void DrawInspectorGui() override;

  private:
    float time_ = 0.0f;
    float current_degree_ = 0.0f;
    float rotate_speed_ = 45.0f;
    float distance_ = 220.0f;
    float line_angle_deg_ = -15.0f;
    float line_length_ = 420.0f;
    float point_angle_deg_ = 50.0f;
    bool animate_point_ = true;
    bool clamp_segment_ = true;
    float last_distance_ = 0.0f;
    float last_proj_t_ = 0.0f;
};

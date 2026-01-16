#pragma once

#include "engine/core/Color4f.h"
#include "engine/math/sgm/public/sgm.h"
#include "engine/scene/IScene.h"

class DotProductScene : public IScene {
  public:
    DotProductScene();
    ~DotProductScene();
    DotProductScene(const DotProductScene&) = delete;
    DotProductScene& operator=(const DotProductScene&) = delete;

    const char* Name() const override { return "DotProductScene"; }
    void OnEnter() override;
    void OnExit() override;
    void Reset() override;
    void Update(const FrameContext& context) override;
    void Render(IRenderer& renderer) override;
    void DrawSceneGui() override;
    void DrawInspectorGui() override;

  private:
    float time_ = 0.0f;
    bool initialized_ = false;
    int last_width_ = 0;
    int last_height_ = 0;
    sgm::vec2 sphere_center_{0.0f, 0.0f};
    Color4f sphere_color_{1.0f, 1.0f, 1.0f, 1.0f};
    Color4f light_color_{1.0f, 0.9f, 0.6f, 1.0f};
    Color4f ambient_color_{1.0f, 1.0f, 1.0f, 1.0f};
    Color4f diffuse_color_{1.0f, 1.0f, 1.0f, 1.0f};
    float ambient_intensity_ = 0.15f;
    float diffuse_intensity_ = 0.9f;
    float light_orbit_scale_ = 0.32f;
    float light_height_scale_ = 1.2f;
    float light_speed_ = 0.6f;
};

#pragma once

#include "engine/scene/IScene.h"

#include <string>
#include <vector>

class Example3DScene : public IScene {
  public:
    Example3DScene();

    const char* Name() const override { return "Example 3D"; }
    void Reset() override;
    void Update(const FrameContext& context) override;
    void Render(IRenderer& renderer) override;
    void DrawSceneGui() override;
    void DrawInspectorGui() override;

  private:
    struct Node {
        std::string name;
        bool is_camera = false;
        enum class Shape { Cube, Pyramid } shape = Shape::Cube;
        float position[3] = {0.0f, 0.0f, 0.0f};
        float rotation[3] = {0.0f, 0.0f, 0.0f};
        float scale[3] = {1.0f, 1.0f, 1.0f};
    };

    std::vector<Node> nodes_;
    size_t selected_index_ = 0;
    bool animate_ = true;
    float spin_speed_ = 0.6f;
    float orbit_speed_ = 0.5f;
    float orbit_radius_ = 2.0f;
    float time_ = 0.0f;
    float fov_degrees_ = 60.0f;
    float near_plane_ = 0.1f;
    float far_plane_ = 100.0f;
    float move_speed_ = 3.0f;
    float look_sensitivity_ = 0.0025f;
    bool has_last_mouse_ = false;
    double last_mouse_x_ = 0.0;
    double last_mouse_y_ = 0.0;
};

#pragma once

#include "engine/scene/IScene.h"
#include "mat2.h"
#include "vec2.h"

#include <vector>

class MatScene : public IScene {
  public:
    MatScene();
    ~MatScene();
    MatScene(const MatScene&) = delete;
    MatScene& operator=(const MatScene&) = delete;

    const char* Name() const override { return "MatScene"; }
    void OnEnter() override;
    void OnExit() override;
    void Reset() override;
    void Update(const FrameContext& context) override;
    void Render(IRenderer& renderer) override;
    void DrawSceneGui() override;
    void DrawInspectorGui() override;

  private:
    float time_ = 0.0f;
    float box_size_;
    std::vector<sgm::vec2> box_;
    bool was_rendered_ = false;
    std::vector<sgm::vec2> vertices_;
    sgm::vec2 box_offset_;
    int last_width_ = 0;
    int last_height_ = 0;

    std::vector<sgm::mat2> matrices_;
    float scale_ = 2.0f;
    float rotation_deg_ = 45.0f;
    float shear_x_ = 2.0f;
    float shear_y_ = 2.0f;
};

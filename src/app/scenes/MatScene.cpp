#include "app/scenes/MatScene.h"

#include "trigonometry.h"
#include "vec2.h"

#include <GLFW/glfw3.h>
#include <cmath>
#include <imgui.h>

MatScene::MatScene() : box_size_(100.0f) {
    box_.reserve(box_size_ * box_size_);
    for (float y = 0.0f; y < box_size_; y += 1.0f) {
        for (float x = 0.0f; x < box_size_; x += 1.0f) {
            box_.push_back({x, y});
        }
    }
}

MatScene::~MatScene() {}

void MatScene::OnEnter() {}

void MatScene::OnExit() {}

void MatScene::Update(const FrameContext& context) { time_ += context.dt; }

void MatScene::Reset() { time_ = 0.0f; }

void MatScene::Render(IRenderer& renderer) {
    int w = renderer.Width();
    int h = renderer.Height();
    if (!was_rendered_ || w != last_width_ || h != last_height_) {
        sgm::vec2 center = {w * 0.5f, h * 0.5f};

        vertices_ = {
            {0, 0},
            {center.x, 0},
            {0, center.y},
            {center.x, center.y},
        };
        box_offset_ = {w * 0.25f, h * 0.25f};
        last_width_ = w;
        last_height_ = h;
        was_rendered_ = true;
    }

    float rad = sgm::radians(rotation_deg_);
    matrices_.clear();
    matrices_.push_back({{scale_, 0}, {0, scale_}});
    matrices_.push_back({{cosf(rad), -sinf(rad)}, {sinf(rad), cosf(rad)}});
    matrices_.push_back({{1, 0}, {shear_x_, 1}});
    matrices_.push_back({{1, shear_y_}, {0, 1}});

    for (int i = 0; i < vertices_.size(); ++i) {
        sgm::vec2 start = vertices_[i] + box_offset_;
        sgm::vec2 half = {box_size_ * 0.5f, box_size_ * 0.5f};

        for (const auto& bv : box_) {
            sgm::vec2 local = bv - half;
            sgm::vec2 pos = matrices_[i] * local + start;
            renderer.PutPixel(pos.x, pos.y, {1.0f, 1.0f, 1.0f, 1.0f});
        }
    }
}

void MatScene::DrawSceneGui() {
    ImGui::Text("Transforms");
    ImGui::Separator();
    ImGui::SliderFloat("Scale", &scale_, 0.25f, 6.0f);
    ImGui::SliderFloat("Rotation (deg)", &rotation_deg_, -180.0f, 180.0f);
    ImGui::SliderFloat("Shear X", &shear_x_, -4.0f, 4.0f);
    ImGui::SliderFloat("Shear Y", &shear_y_, -4.0f, 4.0f);
    if (ImGui::Button("Reset")) {
        scale_ = 2.0f;
        rotation_deg_ = 45.0f;
        shear_x_ = 2.0f;
        shear_y_ = 2.0f;
    }
}

void MatScene::DrawInspectorGui() { ImGui::TextDisabled("No Inspector Data."); }

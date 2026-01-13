#include "app/scenes/AffineScene.h"

#include "trigonometry.h"
#include "vec2.h"
#include "vec3.h"

#include <GLFW/glfw3.h>
#include <cmath>
#include <imgui.h>

AffineScene::AffineScene() : flags{false} {
    float box_size = 100.0f;

    box_.reserve(box_size * box_size);
    for (float y = 0.0f; y < box_size; y += 1.0f) {
        for (float x = 0.0f; x < box_size; x += 1.0f) {
            box_.push_back({x, y, 1});
        }
    }

    S = {
        {2, 0, 0},
        {0, 2, 0},
        {0, 0, 1},
    };

    float deg = 45.0f;
    float rad = sgm::radians(deg);
    R = {
        {cosf(rad), sinf(rad), 0},
        {-sinf(rad), cosf(rad), 0},
        {0, 0, 1},
    };

    T = {
        {1, 0, 0},
        {0, 1, 0},
        {100, 100, 1},
    };
}

AffineScene::~AffineScene() {}

void AffineScene::OnEnter() {}

void AffineScene::OnExit() {}

void AffineScene::Update(const FrameContext& context) { time_ += context.dt; }

void AffineScene::Reset() { time_ = 0.0f; }

void AffineScene::Render(IRenderer& renderer) {
    int w = renderer.Width();
    int h = renderer.Height();
    sgm::vec2 center = {w * 0.5f, h * 0.5f};

    for (const auto& bv : box_) {
        sgm::mat3 transform_mat{1.0f};
        if (flags[0])
            transform_mat = S * transform_mat;
        if (flags[1])
            transform_mat = R * transform_mat;
        if (flags[2])
            transform_mat = T * transform_mat;
        sgm::vec3 pos = transform_mat * bv;
        renderer.PutPixel(pos.x + center.x, pos.y + center.y, {1.0f, 1.0f, 1.0f, 1.0f});
    }
}

void AffineScene::DrawSceneGui() {
    ImGui::TextDisabled("No Scene Data.");
    ImGui::Checkbox("Scale", &flags[0]);
    ImGui::Checkbox("Rotate", &flags[1]);
    ImGui::Checkbox("Translation", &flags[2]);
}

void AffineScene::DrawInspectorGui() { ImGui::TextDisabled("No Inspector Data."); }

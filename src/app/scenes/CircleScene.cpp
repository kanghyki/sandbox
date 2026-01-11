#include "app/scenes/CircleScene.h"

#include "engine/core/Logger.h"

#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <imgui.h>
#include <string>

void CircleScene::Update(const FrameContext& context) {
    time_ += context.dt;

    if (std::abs(prevRadius_ - radius_) >= 1e-8) {
        Logger::Info("Change radius : " + std::to_string(prevRadius_));
        circle_.clear();
        for (float x = -radius_; x <= radius_; ++x) {
            for (float y = -radius_; y <= radius_; ++y) {
                if (x * x + y * y <= radius_ * radius_) {
                    circle_.push_back(sgm::vec2{x, y});
                }
            }
        }
    }

    prevRadius_ = radius_;
}

void CircleScene::Reset() { time_ = 0.0f; }

void CircleScene::Render(IRenderer& renderer) {
    int w = renderer.Width();
    int h = renderer.Height();
    if (w <= 0 || h <= 0) {
        return;
    }
    sgm::vec2 center{w * 0.5f, h * 0.5f};

    for (const auto& v : circle_) {
        renderer.PutPixel(center.x + v.x, center.y + v.y, color_);
    }
}

void CircleScene::DrawSceneGui() { ImGui::TextDisabled("No Scene Data."); }

void CircleScene::DrawInspectorGui() {
    ImGui::SliderFloat("Radius", &radius_, 0.0f, 255.0f);
    ImGui::ColorEdit4("Color", &color_.r);
}

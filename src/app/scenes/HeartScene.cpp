#include "app/scenes/HeartScene.h"

#include "engine/core/Logger.h"
#include "trigonometry.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <imgui.h>

void makeHeart(std::vector<sgm::vec2>& hearts, float increment = 0.0001f) {
    for (float rad = 0.f; rad < sgm::kPi * 2; rad += increment) {
        float sin = sinf(rad);
        float cos = cosf(rad);
        float cos2 = cosf(2 * rad);
        float cos3 = cosf(3 * rad);
        float cos4 = cosf(4 * rad);
        float x = 16.f * sin * sin * sin;
        float y = 13 * cos - 5 * cos2 - 2 * cos3 - cos4;
        hearts.push_back(sgm::vec2(x, y));
    }
}

void HeartScene::OnEnter() {
    makeHeart(hearts_);
    isInitPosition_ = false;
}

void HeartScene::OnExit() {}

void HeartScene::Update(const FrameContext& context) { time_ += context.dt; }

void HeartScene::Reset() { time_ = 0.0f; }

void HeartScene::Render(IRenderer& renderer) {
    if (!isInitPosition_) {
        int w = renderer.Width();
        int h = renderer.Height();
        Logger::Info("width : " + std::to_string(w) + ", height : " + std::to_string(h));
        position_.x = w * 0.5f;
        position_.y = h * 0.5f;
        isInitPosition_ = true;
    }
    float deg = 60.0f * time_;
    normDeg_ = std::fmod(deg, 360.0f);
    float normRadian = sgm::radians(normDeg_);
    float range = (sinf(normRadian) + 1.0f) / 2.0f;
    float scale = amplitude_ * range + scale_;
    for (auto const& v : hearts_) {
        float x = v.x;
        float y = v.y;
        if (isActiveRotation_) {
            x = v.x * cosf(normRadian) - v.y * sinf(normRadian);
            y = v.x * sinf(normRadian) + v.y * cosf(normRadian);
        }
        renderer.PutPixel(x * scale + position_.x, y * scale + position_.y, color_);
    }
}

void HeartScene::DrawSceneGui() { ImGui::Text("Degree : %.2f", normDeg_); }

void HeartScene::DrawInspectorGui() {
    ImGui::DragFloat2("Position", &position_.x);
    ImGui::DragFloat("Scale", &scale_);
    ImGui::DragFloat("Amplitude", &amplitude_);
    ImGui::Checkbox("Is Active Rotation", &isActiveRotation_);
    ImGui::ColorEdit4("Color", &color_.r);
}

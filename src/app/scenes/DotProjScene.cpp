#include "app/scenes/DotProjScene.h"

#include "engine/core/Color4f.h"
#include "engine/math/sgm/public/sgm.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <imgui.h>

namespace {
void DrawLine(IRenderer& renderer, int x0, int y0, int x1, int y1, const Color4f& color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int x = x0;
    int y = y0;
    while (true) {
        renderer.PutPixel(x, y, color);
        if (x == x1 && y == y1) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void DrawFilledCircle(IRenderer& renderer, const sgm::vec2& center, int radius,
                      const Color4f& color) {
    int r2 = radius * radius;
    int cx = static_cast<int>(center.x);
    int cy = static_cast<int>(center.y);
    for (int y = -radius; y <= radius; ++y) {
        int y2 = y * y;
        for (int x = -radius; x <= radius; ++x) {
            if (x * x + y2 <= r2) {
                renderer.PutPixel(cx + x, cy + y, color);
            }
        }
    }
}
} // namespace

DotProjScene::DotProjScene() {}

DotProjScene::~DotProjScene() {}

void DotProjScene::OnEnter() {}

void DotProjScene::OnExit() {}

void DotProjScene::Update(const FrameContext& context) {
    time_ += context.dt;
    if (animate_point_) {
        current_degree_ += rotate_speed_ * context.dt;
        if (current_degree_ >= 360.0f || current_degree_ <= -360.0f) {
            current_degree_ = std::fmod(current_degree_, 360.0f);
        }
    } else {
        current_degree_ = point_angle_deg_;
    }
}

void DotProjScene::Reset() { time_ = 0.0f; }

void DotProjScene::Render(IRenderer& renderer) {
    int w = renderer.Width();
    int h = renderer.Height();
    if (w <= 0 || h <= 0) {
        return;
    }

    sgm::vec2 center{w * 0.5f, h * 0.5f};

    float line_rad = sgm::radians(line_angle_deg_);
    sgm::vec2 line_dir{std::cos(line_rad), std::sin(line_rad)};
    sgm::vec2 line_dir_unit = sgm::normalize(line_dir);
    float half_len = line_length_ * 0.5f;
    sgm::vec2 line_a = center - line_dir_unit * half_len;
    sgm::vec2 line_b = center + line_dir_unit * half_len;

    float point_rad = sgm::radians(current_degree_);
    sgm::vec2 point = center + sgm::vec2{std::cos(point_rad), std::sin(point_rad)} * distance_;

    sgm::vec2 ap = point - line_a;
    float t = sgm::dot(ap, line_dir_unit);
    float segment_len = line_length_;
    float clamped_t = clamp_segment_ ? std::clamp(t, 0.0f, segment_len) : t;
    sgm::vec2 proj = line_a + line_dir_unit * clamped_t;
    last_proj_t_ = clamped_t;
    last_distance_ = sgm::length(point - proj);

    Color4f axis = Color4f::FromBytes(60, 70, 90, 255);
    Color4f line_color = Color4f::FromBytes(230, 230, 230, 255);
    Color4f point_color = Color4f::FromBytes(255, 120, 90, 255);
    Color4f proj_color = Color4f::FromBytes(255, 210, 90, 255);
    Color4f dist_color = Color4f::FromBytes(80, 220, 170, 255);

    DrawLine(renderer, 0, static_cast<int>(center.y), w - 1, static_cast<int>(center.y), axis);
    DrawLine(renderer, static_cast<int>(center.x), 0, static_cast<int>(center.x), h - 1, axis);

    DrawLine(renderer, static_cast<int>(line_a.x), static_cast<int>(line_a.y),
             static_cast<int>(line_b.x), static_cast<int>(line_b.y), line_color);
    DrawLine(renderer, static_cast<int>(point.x), static_cast<int>(point.y),
             static_cast<int>(proj.x), static_cast<int>(proj.y), dist_color);

    DrawFilledCircle(renderer, line_a, 4, line_color);
    DrawFilledCircle(renderer, line_b, 4, line_color);
    DrawFilledCircle(renderer, point, 5, point_color);
    DrawFilledCircle(renderer, proj, 4, proj_color);
}

void DotProjScene::DrawSceneGui() {
    ImGui::Text("Dot Projection - Shortest Distance");
    ImGui::Text("Distance: %.2f", last_distance_);
    ImGui::Text("Projection t: %.2f", last_proj_t_);
    ImGui::Separator();
    ImGui::Checkbox("Animate Point", &animate_point_);
    ImGui::Checkbox("Clamp to Segment", &clamp_segment_);
    ImGui::SliderFloat("Line Angle (deg)", &line_angle_deg_, -180.0f, 180.0f);
    ImGui::SliderFloat("Line Length", &line_length_, 50.0f, 900.0f);
    ImGui::SliderFloat("Point Distance", &distance_, 10.0f, 600.0f);
    if (!animate_point_) {
        ImGui::SliderFloat("Point Angle (deg)", &point_angle_deg_, -180.0f, 180.0f);
    } else {
        ImGui::SliderFloat("Rotate Speed (deg/s)", &rotate_speed_, -360.0f, 360.0f);
    }
}

void DotProjScene::DrawInspectorGui() { ImGui::TextDisabled("No Inspector Data."); }

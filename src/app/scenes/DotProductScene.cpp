#include "app/scenes/DotProductScene.h"

#include "engine/core/Color4f.h"

#include <algorithm>
#include <cmath>
#include <imgui.h>

namespace {
inline void DrawFilledCircle(IRenderer& renderer, const sgm::vec2& center, int radius,
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

inline void DrawLambertSphere(IRenderer& renderer, const sgm::vec2& center, float radius,
                              const sgm::vec3& light_pos, const Color4f& base_color,
                              const Color4f& ambient_color, float ambient,
                              const Color4f& diffuse_color, float diffuse) {
    int r = static_cast<int>(radius);
    int r2 = r * r;
    int cx = static_cast<int>(center.x);
    int cy = static_cast<int>(center.y);
    for (int y = -r; y <= r; ++y) {
        int y2 = y * y;
        for (int x = -r; x <= r; ++x) {
            int x2 = x * x;
            if (x2 + y2 > r2) {
                continue;
            }
            float nx = static_cast<float>(x) / radius;
            float ny = static_cast<float>(y) / radius;
            float nz = std::sqrt(std::max(0.0f, 1.0f - nx * nx - ny * ny));
            sgm::vec3 n{nx, ny, nz};
            sgm::vec3 surface = sgm::vec3{center.x + static_cast<float>(x),
                                          center.y + static_cast<float>(y), nz * radius};
            sgm::vec3 l = sgm::normalize(light_pos - surface);
            float ndotl = std::max(0.0f, sgm::dot(n, l));
            float ambient_term = std::clamp(ambient, 0.0f, 1.0f);
            float diffuse_term = std::clamp(diffuse * ndotl, 0.0f, 1.0f);
            float r =
                base_color.r * (ambient_color.r * ambient_term + diffuse_color.r * diffuse_term);
            float g =
                base_color.g * (ambient_color.g * ambient_term + diffuse_color.g * diffuse_term);
            float b =
                base_color.b * (ambient_color.b * ambient_term + diffuse_color.b * diffuse_term);
            Color4f shaded{std::clamp(r, 0.0f, 1.0f), std::clamp(g, 0.0f, 1.0f),
                           std::clamp(b, 0.0f, 1.0f), 1.0f};
            renderer.PutPixel(cx + x, cy + y, shaded);
        }
    }
}
} // namespace

DotProductScene::DotProductScene() {}

DotProductScene::~DotProductScene() {}

void DotProductScene::OnEnter() {}

void DotProductScene::OnExit() {}

void DotProductScene::Update(const FrameContext& context) { time_ += context.dt; }

void DotProductScene::Reset() { time_ = 0.0f; }

void DotProductScene::Render(IRenderer& renderer) {
    int w = renderer.Width();
    int h = renderer.Height();
    if (w <= 0 || h <= 0) {
        return;
    }

    if (!initialized_ || w != last_width_ || h != last_height_) {
        sphere_center_ = {w * 0.5f, h * 0.5f};
        last_width_ = w;
        last_height_ = h;
        initialized_ = true;
    }

    float radius = static_cast<float>(std::min(w, h)) * 0.20f;
    float orbit = static_cast<float>(std::min(w, h)) * light_orbit_scale_;
    float angle = time_ * light_speed_;
    sgm::vec2 light_xy = sphere_center_ + sgm::vec2{std::cos(angle), std::sin(angle)} * orbit;
    float light_z = radius * light_height_scale_;

    DrawLambertSphere(renderer, sphere_center_, radius, sgm::vec3{light_xy.x, light_xy.y, light_z},
                      sphere_color_, ambient_color_, ambient_intensity_, diffuse_color_,
                      diffuse_intensity_);

    DrawFilledCircle(renderer, light_xy, 6, light_color_);
}

void DotProductScene::DrawSceneGui() {
    ImGui::Text("Lambert Lighting");
    ImGui::SliderFloat("Ambient", &ambient_intensity_, 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse", &diffuse_intensity_, 0.0f, 1.0f);
    ImGui::ColorEdit4("Sphere Color", &sphere_color_.r);
    ImGui::ColorEdit4("Light Color", &light_color_.r);
    ImGui::ColorEdit4("Ambient Color", &ambient_color_.r);
    ImGui::ColorEdit4("Diffuse Color", &diffuse_color_.r);
    ImGui::SliderFloat("Light Orbit", &light_orbit_scale_, 0.05f, 0.6f);
    ImGui::SliderFloat("Light Height", &light_height_scale_, 0.1f, 2.5f);
    ImGui::SliderFloat("Light Speed", &light_speed_, 0.0f, 3.0f);
}

void DotProductScene::DrawInspectorGui() { ImGui::TextDisabled("No Inspector Data."); }

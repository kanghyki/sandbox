#include "app/scenes/Example2DScene.h"

#include "engine/core/Color4f.h"
#include "engine/math/sgm/public/sgm.h"

#include <cmath>

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
} // namespace

void Example2DScene::Update(const FrameContext& context) { time_ += context.dt; }

void Example2DScene::Reset() { time_ = 0.0f; }

void Example2DScene::Render(IRenderer& renderer) {
    int w = renderer.Width();
    int h = renderer.Height();
    if (w <= 0 || h <= 0) {
        return;
    }

    float radius = 0.35f * static_cast<float>(std::min(w, h));
    sgm::vec2 center{w * 0.5f, h * 0.5f};

    float a_angle = 0.35f;
    float b_angle = time_ * 0.7f;

    sgm::vec2 a{std::cos(a_angle) * radius, std::sin(a_angle) * radius};
    sgm::vec2 b{std::cos(b_angle) * radius, std::sin(b_angle) * radius};

    float a_len2 = sgm::dot(a, a);
    float proj_scale = (a_len2 > 0.0f) ? sgm::dot(a, b) / a_len2 : 0.0f;
    sgm::vec2 proj = a * proj_scale;

    sgm::vec2 a_tip = center + a;
    sgm::vec2 b_tip = center + b;
    sgm::vec2 p_tip = center + proj;

    Color4f axis = Color4f::FromBytes(80, 90, 110, 255);
    DrawLine(renderer, 0, static_cast<int>(center.y), w - 1, static_cast<int>(center.y), axis);
    DrawLine(renderer, static_cast<int>(center.x), 0, static_cast<int>(center.x), h - 1, axis);

    DrawLine(renderer, static_cast<int>(center.x), static_cast<int>(center.y),
             static_cast<int>(a_tip.x), static_cast<int>(a_tip.y),
             Color4f::FromBytes(255, 180, 80, 255));

    DrawLine(renderer, static_cast<int>(center.x), static_cast<int>(center.y),
             static_cast<int>(b_tip.x), static_cast<int>(b_tip.y),
             Color4f::FromBytes(80, 200, 190, 255));

    DrawLine(renderer, static_cast<int>(center.x), static_cast<int>(center.y),
             static_cast<int>(p_tip.x), static_cast<int>(p_tip.y),
             Color4f::FromBytes(220, 220, 220, 255));
}

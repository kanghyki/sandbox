#include "app/scenes/DotProductScene.h"

#include "engine/core/Color.h"

#include <algorithm>
#include <cmath>

namespace {
struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

Vec2 Add(const Vec2& a, const Vec2& b) { return Vec2{a.x + b.x, a.y + b.y}; }

Vec2 Scale(const Vec2& v, float s) { return Vec2{v.x * s, v.y * s}; }

float Dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }

void DrawLine(IRenderer& renderer, int x0, int y0, int x1, int y1, Color color) {
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

void DotProductScene::Update(float dt) { time_ += dt; }

void DotProductScene::Reset() { time_ = 0.0f; }

void DotProductScene::Render(IRenderer& renderer) {
    int w = renderer.Width();
    int h = renderer.Height();
    if (w <= 0 || h <= 0) {
        return;
    }

    float radius = 0.35f * static_cast<float>(std::min(w, h));
    Vec2 center{w * 0.5f, h * 0.5f};

    float a_angle = 0.35f;
    float b_angle = time_ * 0.7f;

    Vec2 a{std::cos(a_angle) * radius, std::sin(a_angle) * radius};
    Vec2 b{std::cos(b_angle) * radius, std::sin(b_angle) * radius};

    float a_len2 = Dot(a, a);
    float proj_scale = (a_len2 > 0.0f) ? Dot(a, b) / a_len2 : 0.0f;
    Vec2 proj = Scale(a, proj_scale);

    Vec2 a_tip = Add(center, a);
    Vec2 b_tip = Add(center, b);
    Vec2 p_tip = Add(center, proj);

    Color axis = ColorRGBA(80, 90, 110, 255);
    DrawLine(renderer, 0, static_cast<int>(center.y), w - 1, static_cast<int>(center.y), axis);
    DrawLine(renderer, static_cast<int>(center.x), 0, static_cast<int>(center.x), h - 1, axis);

    DrawLine(renderer, static_cast<int>(center.x), static_cast<int>(center.y),
             static_cast<int>(a_tip.x), static_cast<int>(a_tip.y), ColorRGBA(255, 180, 80, 255));

    DrawLine(renderer, static_cast<int>(center.x), static_cast<int>(center.y),
             static_cast<int>(b_tip.x), static_cast<int>(b_tip.y), ColorRGBA(80, 200, 190, 255));

    DrawLine(renderer, static_cast<int>(center.x), static_cast<int>(center.y),
             static_cast<int>(p_tip.x), static_cast<int>(p_tip.y), ColorRGBA(220, 220, 220, 255));
}

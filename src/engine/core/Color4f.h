#pragma once

#include <cstdint>

struct Color4f {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;

    constexpr Color4f() = default;
    constexpr Color4f(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

    static constexpr Color4f FromBytes(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        constexpr float inv = 1.0f / 255.0f;
        return Color4f(r * inv, g * inv, b * inv, a * inv);
    }
};

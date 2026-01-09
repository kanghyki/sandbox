#include "engine/render/PixelRenderer.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace {
constexpr size_t kLutSize = 4096;

const std::array<uint8_t, kLutSize>& GetColorLut() {
    static const std::array<uint8_t, kLutSize> lut = [] {
        std::array<uint8_t, kLutSize> table{};
        for (size_t i = 0; i < kLutSize; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(kLutSize - 1);
            table[i] = static_cast<uint8_t>(std::round(t * 255.0f));
        }
        return table;
    }();
    return lut;
}

uint8_t ToByteLut(float v) {
    float clamped = std::clamp(v, 0.0f, 1.0f);
    float scaled = clamped * static_cast<float>(kLutSize - 1);
    size_t idx = static_cast<size_t>(scaled + 0.5f);
    return GetColorLut()[idx];
}

void Vec4ToColor(const sgm::vec4& color, uint8_t* out) {
    out[0] = ToByteLut(color.r);
    out[1] = ToByteLut(color.g);
    out[2] = ToByteLut(color.b);
    out[3] = ToByteLut(color.a);
}
} // namespace

PixelRenderer::PixelRenderer(int width, int height) { Resize(width, height); }

void PixelRenderer::Resize(int width, int height) {
    width_ = std::max(0, width);
    height_ = std::max(0, height);
    pixels_.assign(static_cast<size_t>(width_ * height_ * 4), 0);
}

void PixelRenderer::Clear(const sgm::vec4& color) {
    uint8_t rgba[4];
    Vec4ToColor(color, rgba);
    for (size_t i = 0; i + 3 < pixels_.size(); i += 4) {
        pixels_[i] = rgba[0];
        pixels_[i + 1] = rgba[1];
        pixels_[i + 2] = rgba[2];
        pixels_[i + 3] = rgba[3];
    }
}

void PixelRenderer::PutPixel(int x, int y, const sgm::vec4& color) {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        return;
    }
    uint8_t rgba[4];
    Vec4ToColor(color, rgba);
    size_t index = static_cast<size_t>((y * width_ + x) * 4);
    pixels_[index] = rgba[0];
    pixels_[index + 1] = rgba[1];
    pixels_[index + 2] = rgba[2];
    pixels_[index + 3] = rgba[3];
}

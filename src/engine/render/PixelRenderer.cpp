#include "engine/render/PixelRenderer.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>

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
} // namespace

PixelRenderer::PixelRenderer(int width, int height) { Resize(width, height); }

PixelRenderer::Pixel PixelRenderer::ColorToPixel(const Color4f& color) {
    return Pixel{ToByteLut(color.r), ToByteLut(color.g), ToByteLut(color.b), ToByteLut(color.a)};
}

void PixelRenderer::Resize(int width, int height) {
    width_ = std::max(0, width);
    height_ = std::max(0, height);
    pixels_.assign(static_cast<size_t>(width_ * height_), Pixel{});
    clear_row_.clear();
    clear_row_valid_ = false;
}

void PixelRenderer::Clear(const Color4f& color) {
    Pixel rgba = ColorToPixel(color);
    if (!clear_row_valid_ || !(clear_row_pixel_ == rgba) ||
        clear_row_.size() != static_cast<size_t>(width_)) {
        clear_row_.assign(static_cast<size_t>(width_), rgba);
        clear_row_pixel_ = rgba;
        clear_row_valid_ = true;
    }
    if (width_ <= 0 || height_ <= 0) {
        return;
    }
    Pixel* dst = pixels_.data();
    const size_t row_bytes = static_cast<size_t>(width_) * sizeof(Pixel);
    for (int y = 0; y < height_; ++y) {
        std::memcpy(dst, clear_row_.data(), row_bytes);
        dst += width_;
    }
}

void PixelRenderer::PutPixel(int x, int y, const Color4f& color) {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        return;
    }
    Pixel rgba = ColorToPixel(color);
    size_t index = static_cast<size_t>(y * width_ + x);
    pixels_[index] = rgba;
}

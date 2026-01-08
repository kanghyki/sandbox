#include "engine/render/PixelRenderer.h"

#include <algorithm>

PixelRenderer::PixelRenderer(int width, int height) { Resize(width, height); }

void PixelRenderer::Resize(int width, int height) {
    width_ = std::max(0, width);
    height_ = std::max(0, height);
    pixels_.assign(static_cast<size_t>(width_ * height_ * 4), 0);
}

void PixelRenderer::Clear(Color color) {
    for (size_t i = 0; i + 3 < pixels_.size(); i += 4) {
        pixels_[i] = color.r;
        pixels_[i + 1] = color.g;
        pixels_[i + 2] = color.b;
        pixels_[i + 3] = color.a;
    }
}

void PixelRenderer::PutPixel(int x, int y, Color color) {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        return;
    }
    size_t index = static_cast<size_t>((y * width_ + x) * 4);
    pixels_[index] = color.r;
    pixels_[index + 1] = color.g;
    pixels_[index + 2] = color.b;
    pixels_[index + 3] = color.a;
}

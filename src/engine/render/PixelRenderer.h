#pragma once

#include "engine/core/IRenderer.h"

#include <cstdint>
#include <vector>

class PixelRenderer : public IRenderer {
  public:
    PixelRenderer(int width, int height);

    void Resize(int width, int height) override;
    void Clear(const Color4f& color) override;
    void PutPixel(int x, int y, const Color4f& color) override;

    int Width() const override { return width_; }
    int Height() const override { return height_; }

    const uint8_t* Pixels() const override {
        return reinterpret_cast<const uint8_t*>(pixels_.data());
    }

  private:
    struct Pixel {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 255;

        bool operator==(const Pixel& other) const {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }
    };

    static Pixel ColorToPixel(const Color4f& color);

    int width_ = 0;
    int height_ = 0;

    std::vector<Pixel> pixels_;
    std::vector<Pixel> clear_row_;
    Pixel clear_row_pixel_{};
    bool clear_row_valid_ = false;
};

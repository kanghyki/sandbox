#pragma once

#include "engine/core/IRenderer.h"

class PixelRenderer : public IRenderer {
  public:
    PixelRenderer(int width, int height);

    void Resize(int width, int height) override;
    void Clear(const sgm::vec4& color) override;
    void PutPixel(int x, int y, const sgm::vec4& color) override;

    int Width() const override { return width_; }
    int Height() const override { return height_; }

    const std::vector<uint8_t>& Pixels() const override { return pixels_; }

  private:
    int width_ = 0;
    int height_ = 0;

    std::vector<uint8_t> pixels_;
};

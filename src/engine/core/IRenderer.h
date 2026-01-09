#pragma once

#include "engine/core/Color4f.h"

#include <cstdint>

class IRenderer {
  public:
    virtual ~IRenderer() = default;

    virtual void Resize(int width, int height) = 0;
    virtual void Clear(const Color4f& color) = 0;
    virtual void PutPixel(int x, int y, const Color4f& color) = 0;

    virtual int Width() const = 0;
    virtual int Height() const = 0;
    virtual const uint8_t* Pixels() const = 0;
};

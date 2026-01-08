#pragma once

#include "engine/core/Color.h"

#include <cstdint>
#include <vector>

class IRenderer {
  public:
    virtual ~IRenderer() = default;

    virtual void Resize(int width, int height) = 0;
    virtual void Clear(Color color) = 0;
    virtual void PutPixel(int x, int y, Color color) = 0;

    virtual int Width() const = 0;
    virtual int Height() const = 0;
    virtual const std::vector<uint8_t>& Pixels() const = 0;
};

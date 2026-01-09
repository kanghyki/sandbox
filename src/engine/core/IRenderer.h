#pragma once

#include "engine/math/sgm/public/vec4.h"

#include <cstdint>
#include <vector>

class IRenderer {
  public:
    virtual ~IRenderer() = default;

    virtual void Resize(int width, int height) = 0;
    virtual void Clear(const sgm::vec4& color) = 0;
    virtual void PutPixel(int x, int y, const sgm::vec4& color) = 0;

    virtual int Width() const = 0;
    virtual int Height() const = 0;
    virtual const std::vector<uint8_t>& Pixels() const = 0;
};

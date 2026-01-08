#pragma once

#include "engine/core/IRenderer.h"

class IScene {
  public:
    virtual ~IScene() = default;
    virtual const char* Name() const = 0;
    virtual void Reset() {}
    virtual void Update(float dt) { (void)dt; }
    virtual void Render(IRenderer& renderer) = 0;
};

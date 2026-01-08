#pragma once

#include "engine/core/IRenderer.h"
#include "engine/scene/FrameContext.h"

class IScene {
  public:
    virtual ~IScene() = default;
    virtual const char* Name() const = 0;
    virtual void Reset() {}
    virtual void Update(const FrameContext& context) { (void)context; }
    virtual void Render(IRenderer& renderer) = 0;
    virtual void DrawSceneGui() {}
    virtual void DrawInspectorGui() {}
};

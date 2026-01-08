#pragma once

#include "engine/core/InputState.h"

class IWindow {
  public:
    virtual ~IWindow() = default;

    virtual bool IsValid() const = 0;
    virtual bool ShouldClose() const = 0;
    virtual void SetShouldClose(bool value) = 0;
    virtual void SetFullscreen(bool enabled) = 0;
    virtual bool IsFullscreen() const = 0;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual void GetFramebufferSize(int* width, int* height) const = 0;
    virtual void* NativeHandle() const = 0;

    virtual InputState& Input() = 0;
    virtual const InputState& Input() const = 0;
};

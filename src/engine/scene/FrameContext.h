#pragma once

#include "engine/core/InputState.h"

struct FrameContext {
    float dt = 0.0f;
    const InputState* input = nullptr;
    bool viewport_hovered = false;
};

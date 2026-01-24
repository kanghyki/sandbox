#pragma once

#include "engine/core/IRenderer.h"

#include <cstdint>
#include <imgui.h>

class GlPresenter {
  public:
    bool Init();
    void Shutdown();
    void Resize(int width, int height);
    bool Upload(const IRenderer& renderer);
    void DrawFullscreen();
    void Present(const IRenderer& renderer);
    ImTextureID TextureId() const { return (ImTextureID)(intptr_t)texture_; }

  private:
    unsigned int program_ = 0;
    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    unsigned int ebo_ = 0;
    unsigned int texture_ = 0;
    int tex_width_ = 0;
    int tex_height_ = 0;
    bool initialized_ = false;
};

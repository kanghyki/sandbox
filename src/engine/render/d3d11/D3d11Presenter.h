#pragma once

#include "engine/core/IRenderer.h"

#include <cstdint>

#ifdef _WIN32
#include <GLFW/glfw3.h>
#include <d3d11.h>
#include <wrl/client.h>
#endif

class D3d11Presenter {
  public:
    bool Init(GLFWwindow* window);
    void Shutdown();
    void Resize(int width, int height);
    bool Upload(const IRenderer& renderer);
    void BeginFrame(const float clear_color[4]);
    void EndFrame();
    void Present(bool vsync);
    void* TextureId() const { return texture_srv_.Get(); }

    ID3D11Device* Device() const { return device_.Get(); }
    ID3D11DeviceContext* Context() const { return context_.Get(); }

  private:
    void CreateRenderTarget();
    void DestroyRenderTarget();
    void UpdateViewport(int width, int height);
    bool CreateTexture(int width, int height);

    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_srv_;
    int tex_width_ = 0;
    int tex_height_ = 0;
    int backbuffer_width_ = 0;
    int backbuffer_height_ = 0;
    bool initialized_ = false;
};

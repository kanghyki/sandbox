#include "engine/render/d3d11/D3d11Presenter.h"

#ifdef _WIN32
#include <GLFW/glfw3native.h>
#include <cstring>
#include <dxgi.h>
#include <iterator>

namespace {
DXGI_FORMAT kBackbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
}

bool D3d11Presenter::Init(GLFWwindow* window) {
    if (initialized_) {
        return true;
    }
    if (!window) {
        return false;
    }

    HWND hwnd = glfwGetWin32Window(window);
    if (!hwnd) {
        return false;
    }

    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferDesc.Format = kBackbufferFormat;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.OutputWindow = hwnd;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT create_flags = 0;
    D3D_FEATURE_LEVEL feature_levels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
                                          D3D_FEATURE_LEVEL_10_0};
    D3D_FEATURE_LEVEL created_level = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_flags, feature_levels,
        static_cast<UINT>(std::size(feature_levels)), D3D11_SDK_VERSION, &desc,
        swap_chain_.GetAddressOf(), device_.GetAddressOf(), &created_level,
        context_.GetAddressOf());
    if (FAILED(hr)) {
        return false;
    }

    CreateRenderTarget();
    initialized_ = true;
    return true;
}

void D3d11Presenter::Shutdown() {
    if (!initialized_) {
        return;
    }
    texture_srv_.Reset();
    texture_.Reset();
    DestroyRenderTarget();
    swap_chain_.Reset();
    context_.Reset();
    device_.Reset();
    initialized_ = false;
}

void D3d11Presenter::Resize(int width, int height) {
    if (!initialized_) {
        return;
    }
    if (width <= 0 || height <= 0) {
        return;
    }
    if (width == backbuffer_width_ && height == backbuffer_height_) {
        return;
    }

    DestroyRenderTarget();
    HRESULT hr = swap_chain_->ResizeBuffers(0, static_cast<UINT>(width), static_cast<UINT>(height),
                                            kBackbufferFormat, 0);
    if (FAILED(hr)) {
        return;
    }
    CreateRenderTarget();
    UpdateViewport(width, height);
}

bool D3d11Presenter::CreateTexture(int width, int height) {
    if (width <= 0 || height <= 0) {
        return false;
    }

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = static_cast<UINT>(width);
    desc.Height = static_cast<UINT>(height);
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = device_->CreateTexture2D(&desc, nullptr, texture.GetAddressOf());
    if (FAILED(hr)) {
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    hr = device_->CreateShaderResourceView(texture.Get(), &srv_desc, srv.GetAddressOf());
    if (FAILED(hr)) {
        return false;
    }

    texture_ = texture;
    texture_srv_ = srv;
    tex_width_ = width;
    tex_height_ = height;
    return true;
}

bool D3d11Presenter::Upload(const IRenderer& renderer) {
    if (!initialized_) {
        return false;
    }
    const int width = renderer.Width();
    const int height = renderer.Height();
    if (width <= 0 || height <= 0) {
        return false;
    }

    if (!texture_ || width != tex_width_ || height != tex_height_) {
        if (!CreateTexture(width, height)) {
            return false;
        }
    }

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    HRESULT hr = context_->Map(texture_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (FAILED(hr)) {
        return false;
    }

    const uint8_t* src = renderer.Pixels();
    const size_t row_bytes = static_cast<size_t>(width) * 4;
    uint8_t* dst = static_cast<uint8_t*>(mapped.pData);
    for (int y = 0; y < height; ++y) {
        std::memcpy(dst + static_cast<size_t>(y) * mapped.RowPitch,
                    src + static_cast<size_t>(y) * row_bytes, row_bytes);
    }

    context_->Unmap(texture_.Get(), 0);
    return true;
}

void D3d11Presenter::BeginFrame(const float clear_color[4]) {
    if (!initialized_) {
        return;
    }
    if (!render_target_) {
        return;
    }
    context_->OMSetRenderTargets(1, render_target_.GetAddressOf(), nullptr);
    context_->ClearRenderTargetView(render_target_.Get(), clear_color);
}

void D3d11Presenter::EndFrame() {}

void D3d11Presenter::Present(bool vsync) {
    if (!initialized_) {
        return;
    }
    swap_chain_->Present(vsync ? 1 : 0, 0);
}

void D3d11Presenter::CreateRenderTarget() {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
    HRESULT hr = swap_chain_->GetBuffer(0, IID_PPV_ARGS(back_buffer.GetAddressOf()));
    if (FAILED(hr)) {
        return;
    }
    device_->CreateRenderTargetView(back_buffer.Get(), nullptr, render_target_.GetAddressOf());

    D3D11_TEXTURE2D_DESC desc = {};
    back_buffer->GetDesc(&desc);
    backbuffer_width_ = static_cast<int>(desc.Width);
    backbuffer_height_ = static_cast<int>(desc.Height);
    UpdateViewport(backbuffer_width_, backbuffer_height_);
}

void D3d11Presenter::DestroyRenderTarget() { render_target_.Reset(); }

void D3d11Presenter::UpdateViewport(int width, int height) {
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context_->RSSetViewports(1, &viewport);
}
#endif

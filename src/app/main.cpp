#include "app/scenes/SceneRegistry.h"
#include "engine/core/Color4f.h"
#include "engine/core/IRenderer.h"
#include "engine/core/IWindow.h"
#include "engine/core/Logger.h"
#include "engine/platform/glfw/GlfwWindow.h"
#include "engine/render/PixelRenderer.h"
#if defined(SANDBOX_D3D11)
#include "engine/render/d3d11/D3d11Presenter.h"
#else
#include "engine/render/opengl/GlPresenter.h"
#endif
#include "engine/scene/FrameContext.h"
#include "engine/scene/SceneManager.h"
#include "engine/ui/EditorUi.h"
#include "engine/ui/ImGuiLayer.h"

#include <GLFW/glfw3.h>
#include <cstdint>
#include <iostream>
#include <memory>


namespace {
EditorUi g_editor_ui{};

struct AppState {
    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;
#if defined(SANDBOX_D3D11)
    D3d11Presenter presenter;
#else
    GlPresenter presenter;
#endif
    ImGuiLayer imgui;
    SceneManager scenes;
    double last_time = 0.0;
    bool initialized = false;

    bool Init() {
        window = std::make_unique<GlfwWindow>(960, 600, "Sandbox");
        if (!window || !window->IsValid()) {
            Logger::Error("Failed to create GLFW window.");
            return false;
        }

        GLFWwindow* glfw_window = static_cast<GLFWwindow*>(window->NativeHandle());
        if (!glfw_window) {
            return false;
        }
        glfwMaximizeWindow(glfw_window);

        int fb_width = 0;
        int fb_height = 0;
        window->GetFramebufferSize(&fb_width, &fb_height);
        renderer = std::make_unique<PixelRenderer>(fb_width, fb_height);

#if defined(SANDBOX_D3D11)
        if (!presenter.Init(glfw_window)) {
            std::cerr << "Failed to initialize D3D11 presenter\n";
            Logger::Error("Failed to initialize D3D11 presenter.");
            return false;
        }

        if (!imgui.Init(glfw_window, presenter.Device(), presenter.Context())) {
            std::cerr << "Failed to initialize ImGui (D3D11)\n";
            Logger::Error("Failed to initialize ImGui (D3D11).");
            return false;
        }
#else
        if (!presenter.Init()) {
            std::cerr << "Failed to initialize OpenGL presenter\n";
            Logger::Error("Failed to initialize OpenGL presenter.");
            return false;
        }

        if (!imgui.Init(glfw_window)) {
            std::cerr << "Failed to initialize ImGui\n";
            Logger::Error("Failed to initialize ImGui.");
            return false;
        }
#endif

        Logger::Info("Editor initialized.");

        RegisterScenes(scenes);

        last_time = glfwGetTime();
        initialized = true;
        return true;
    }

    bool Frame() {
        if (!initialized) {
            return false;
        }
        if (window->ShouldClose()) {
            return false;
        }

        window->PollEvents();

        int fb_width = 0;
        int fb_height = 0;
        window->GetFramebufferSize(&fb_width, &fb_height);
        if (fb_width == 0 || fb_height == 0) {
            return true;
        }

        int desired_width = g_editor_ui.ViewportTargetWidth();
        int desired_height = g_editor_ui.ViewportTargetHeight();
        if (desired_width != renderer->Width() || desired_height != renderer->Height()) {
            renderer->Resize(desired_width, desired_height);
        }

        int win_width = 0;
        int win_height = 0;
        if (GLFWwindow* raw = static_cast<GLFWwindow*>(window->NativeHandle())) {
            glfwGetWindowSize(raw, &win_width, &win_height);
        }

        const InputState& input = window->Input();
        if (input.IsKeyDown(GLFW_KEY_ESCAPE)) {
            window->SetShouldClose(true);
        }

        if (window->IsVsync() != g_editor_ui.VsyncEnabled()) {
            window->SetVsync(g_editor_ui.VsyncEnabled());
        }

        const float* clear_color = g_editor_ui.ClearColor();
        renderer->Clear(Color4f{clear_color[0], clear_color[1], clear_color[2], clear_color[3]});

        double now = glfwGetTime();
        float dt = static_cast<float>(now - last_time);
        last_time = now;
        bool advance = true;
        bool step = false;
        EditorUi::PlayState play_state = g_editor_ui.GetPlayState();
        if (play_state == EditorUi::PlayState::Paused) {
            advance = false;
            step = g_editor_ui.ConsumeStepRequested();
        } else if (play_state == EditorUi::PlayState::Stopped) {
            advance = false;
            if (g_editor_ui.ConsumeStopRequested()) {
                if (IScene* scene = scenes.ActiveScene()) {
                    scene->Reset();
                }
            }
        }

        float update_dt = dt;
        if (step) {
            update_dt = 1.0f / 60.0f;
        }

        if (IScene* scene = scenes.ActiveScene()) {
            if (advance || step) {
                FrameContext context;
                context.dt = update_dt;
                context.input = &input;
                context.viewport_hovered = g_editor_ui.IsViewportHovered();
                scene->Update(context);
            }
            scene->Render(*renderer);
        }

        int viewport_mouse_x = 0;
        int viewport_mouse_y = 0;
        bool has_viewport_mouse =
            g_editor_ui.GetViewportMousePixel(&viewport_mouse_x, &viewport_mouse_y);
        bool left_down = input.IsMouseDown(GLFW_MOUSE_BUTTON_LEFT);
        static bool was_left_down = false;
        if (has_viewport_mouse && left_down && !was_left_down) {
            Logger::Info("Viewport click at (" + std::to_string(viewport_mouse_x) + ", " +
                         std::to_string(viewport_mouse_y) + ")");
        }
        was_left_down = left_down;

        if (has_viewport_mouse) {
            Color4f cursor_color = left_down ? Color4f::FromBytes(64, 200, 120, 255)
                                             : Color4f::FromBytes(240, 120, 120, 255);

            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    renderer->PutPixel(viewport_mouse_x + dx, viewport_mouse_y + dy, cursor_color);
                }
            }
        }

        presenter.Upload(*renderer);

#if defined(SANDBOX_D3D11)
        presenter.Resize(fb_width, fb_height);
        presenter.BeginFrame(clear_color);
#else
        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);
#endif

        imgui.BeginFrame();
        g_editor_ui.Draw(presenter.TextureId(), renderer->Width(), renderer->Height(), win_width,
                         win_height, scenes);
        imgui.EndFrame();

#if defined(SANDBOX_D3D11)
        presenter.EndFrame();
        presenter.Present(window->IsVsync());
#else
        window->SwapBuffers();
#endif
        return !window->ShouldClose();
    }

    void Shutdown() {
        if (!initialized) {
            return;
        }
        imgui.Shutdown();
        presenter.Shutdown();
        renderer.reset();
        window.reset();
        initialized = false;
    }
};

std::unique_ptr<AppState> g_app;

} // namespace

int main(int argc, char** argv) {
    g_app = std::make_unique<AppState>();
    if (!g_app->Init()) {
        g_app.reset();
        return 1;
    }

    while (g_app->Frame()) {}
    g_app->Shutdown();
    g_app.reset();
    return 0;
}

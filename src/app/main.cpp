#include "app/scenes/SceneRegistry.h"
#include "engine/core/Color4f.h"
#include "engine/core/IRenderer.h"
#include "engine/core/IWindow.h"
#include "engine/core/Logger.h"
#include "engine/platform/glfw/GlfwWindow.h"
#include "engine/render/PixelRenderer.h"
#include "engine/render/opengl/GlPresenter.h"
#include "engine/scene/FrameContext.h"
#include "engine/scene/SceneManager.h"
#include "engine/ui/EditorUi.h"
#include "engine/ui/ImGuiLayer.h"

#include <GLFW/glfw3.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

namespace {
EditorUi g_editor_ui{};
} // namespace

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    std::unique_ptr<IWindow> window = std::make_unique<GlfwWindow>(960, 600, "Sandbox");
    if (!window->IsValid()) {
        Logger::Error("Failed to create GLFW window.");
        return 1;
    }

    GLFWwindow* glfw_window = static_cast<GLFWwindow*>(window->NativeHandle());
    if (!glfw_window) {
        return 1;
    }

    int fb_width = 0;
    int fb_height = 0;
    window->GetFramebufferSize(&fb_width, &fb_height);
    std::unique_ptr<IRenderer> renderer = std::make_unique<PixelRenderer>(fb_width, fb_height);

    GlPresenter presenter;
    if (!presenter.Init()) {
        std::cerr << "Failed to initialize OpenGL presenter\n";
        Logger::Error("Failed to initialize OpenGL presenter.");
        return 1;
    }

    ImGuiLayer imgui;
    if (!imgui.Init(glfw_window)) {
        std::cerr << "Failed to initialize ImGui\n";
        Logger::Error("Failed to initialize ImGui.");
        return 1;
    }

    Logger::Info("Editor initialized.");

    SceneManager scenes;
    RegisterScenes(scenes);

    double last_time = glfwGetTime();

    while (!window->ShouldClose()) {
        window->PollEvents();

        window->GetFramebufferSize(&fb_width, &fb_height);
        if (fb_width == 0 || fb_height == 0) {
            continue;
        }

        if (fb_width != renderer->Width() || fb_height != renderer->Height()) {
            renderer->Resize(fb_width, fb_height);
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

        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        imgui.BeginFrame();
        g_editor_ui.Draw(presenter.TextureId(), renderer->Width(), renderer->Height(), win_width,
                         win_height, scenes);
        imgui.EndFrame();

        window->SwapBuffers();
    }

    imgui.Shutdown();
    presenter.Shutdown();
    return 0;
}

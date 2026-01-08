#include "app/scenes/SceneRegistry.h"
#include "engine/core/Color.h"
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

void DrawLine(IRenderer& renderer, int x0, int y0, int x1, int y1, Color color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int x = x0;
    int y = y0;
    while (true) {
        renderer.PutPixel(x, y, color);
        if (x == x1 && y == y1) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

} // namespace

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    std::unique_ptr<IWindow> window = std::make_unique<GlfwWindow>(800, 600, "Sandbox");
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

        const InputState& input = window->Input();
        if (input.IsKeyDown(GLFW_KEY_ESCAPE)) {
            window->SetShouldClose(true);
        }

        const float* clear_color = g_editor_ui.ClearColor();
        auto to_byte = [](float v) -> uint8_t {
            if (v < 0.0f) {
                return 0;
            }
            if (v > 1.0f) {
                return 255;
            }
            return static_cast<uint8_t>(v * 255.0f);
        };
        renderer->Clear(ColorRGBA(to_byte(clear_color[0]), to_byte(clear_color[1]),
                                  to_byte(clear_color[2]), 255));

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
            Color cursor_color =
                left_down ? ColorRGBA(64, 200, 120, 255) : ColorRGBA(240, 120, 120, 255);

            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    renderer->PutPixel(viewport_mouse_x + dx, viewport_mouse_y + dy, cursor_color);
                }
            }
        }

        presenter.Upload(*renderer);

        glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        imgui.BeginFrame();
        g_editor_ui.Draw(presenter.TextureId(), renderer->Width(), renderer->Height(), scenes);
        imgui.EndFrame();

        window->SwapBuffers();
    }

    imgui.Shutdown();
    presenter.Shutdown();
    return 0;
}

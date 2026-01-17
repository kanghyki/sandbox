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
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace {
EditorUi g_editor_ui{};

struct SceneSelection {
    bool has_selection = false;
    bool by_index = false;
    size_t index = 0;
    std::string name;
};

bool IsNumber(const std::string& value) {
    if (value.empty()) {
        return false;
    }
    for (char c : value) {
        if (c < '0' || c > '9') {
            return false;
        }
    }
    return true;
}

SceneSelection ParseSceneSelection(int argc, char** argv) {
    SceneSelection selection;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i] ? argv[i] : "";
        if (arg == "--scene" && i + 1 < argc) {
            selection.has_selection = true;
            selection.by_index = false;
            selection.name = argv[++i] ? argv[i] : "";
        } else if (arg.rfind("--scene=", 0) == 0) {
            selection.has_selection = true;
            selection.by_index = false;
            selection.name = arg.substr(std::string("--scene=").size());
        } else if (arg == "--scene-index" && i + 1 < argc) {
            selection.has_selection = true;
            selection.by_index = true;
            selection.index = static_cast<size_t>(std::max(0, std::atoi(argv[++i])));
        } else if (arg.rfind("--scene-index=", 0) == 0) {
            selection.has_selection = true;
            selection.by_index = true;
            std::string value = arg.substr(std::string("--scene-index=").size());
            selection.index = static_cast<size_t>(std::max(0, std::atoi(value.c_str())));
        } else if (arg.rfind("--scene_index=", 0) == 0) {
            selection.has_selection = true;
            selection.by_index = true;
            std::string value = arg.substr(std::string("--scene_index=").size());
            selection.index = static_cast<size_t>(std::max(0, std::atoi(value.c_str())));
        }
    }
    return selection;
}

size_t FindSceneIndexByName(SceneManager& scenes, const std::string& name) {
    size_t count = scenes.SceneCount();
    for (size_t i = 0; i < count; ++i) {
        const IScene* scene = scenes.GetScene(i);
        if (scene && name == scene->Name()) {
            return i;
        }
    }
    return static_cast<size_t>(-1);
}

struct AppState {
    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;
    GlPresenter presenter;
    ImGuiLayer imgui;
    SceneManager scenes;
    double last_time = 0.0;
    bool initialized = false;

    bool Init(int argc, char** argv) {
        window = std::make_unique<GlfwWindow>(960, 600, "Sandbox");
        if (!window || !window->IsValid()) {
            Logger::Error("Failed to create GLFW window.");
            return false;
        }

        GLFWwindow* glfw_window = static_cast<GLFWwindow*>(window->NativeHandle());
        if (!glfw_window) {
            return false;
        }
#ifndef __EMSCRIPTEN__
        glfwMaximizeWindow(glfw_window);
#endif

        int fb_width = 0;
        int fb_height = 0;
        window->GetFramebufferSize(&fb_width, &fb_height);
        renderer = std::make_unique<PixelRenderer>(fb_width, fb_height);

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

        Logger::Info("Editor initialized.");

        RegisterScenes(scenes);
        SceneSelection selection = ParseSceneSelection(argc, argv);
        if (selection.has_selection) {
            g_editor_ui.SetFocusViewport(true);
            if (selection.by_index) {
                if (selection.index < scenes.SceneCount()) {
                    scenes.SetActiveIndex(selection.index);
                    Logger::Info("Scene selected by index: " + std::to_string(selection.index));
                } else {
                    Logger::Warn("Scene index out of range: " + std::to_string(selection.index));
                }
            } else if (!selection.name.empty()) {
                size_t idx = FindSceneIndexByName(scenes, selection.name);
                if (idx != static_cast<size_t>(-1)) {
                    scenes.SetActiveIndex(idx);
                    Logger::Info("Scene selected by name: " + selection.name);
                } else if (IsNumber(selection.name)) {
                    size_t numeric = static_cast<size_t>(std::atoi(selection.name.c_str()));
                    if (numeric < scenes.SceneCount()) {
                        scenes.SetActiveIndex(numeric);
                        Logger::Info("Scene selected by numeric name: " + std::to_string(numeric));
                    } else {
                        Logger::Warn("Scene numeric name out of range: " + selection.name);
                    }
                } else {
                    Logger::Warn("Scene not found: " + selection.name);
                }
            }
        }

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

        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        imgui.BeginFrame();
        g_editor_ui.Draw(presenter.TextureId(), renderer->Width(), renderer->Height(), win_width,
                         win_height, scenes);
        imgui.EndFrame();

        window->SwapBuffers();
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

#ifdef __EMSCRIPTEN__
void MainLoop(void* arg) {
    auto* app = static_cast<AppState*>(arg);
    if (!app->Frame()) {
        app->Shutdown();
        emscripten_cancel_main_loop();
    }
}
#endif
} // namespace

int main(int argc, char** argv) {
    g_app = std::make_unique<AppState>();
    if (!g_app->Init(argc, argv)) {
        g_app.reset();
        return 1;
    }

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(MainLoop, g_app.get(), 0, true);
    return 0;
#else
    while (g_app->Frame()) {}
    g_app->Shutdown();
    g_app.reset();
    return 0;
#endif
}

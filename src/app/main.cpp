#include "engine/core/Color.h"
#include "engine/core/IRenderer.h"
#include "engine/core/IWindow.h"
#include "engine/platform/glfw/GlfwWindow.h"
#include "engine/render/PixelRenderer.h"
#include "engine/render/opengl/GlPresenter.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    std::unique_ptr<IWindow> window = std::make_unique<GlfwWindow>(800, 600, "Sandbox");
    if (!window->IsValid()) {
        return 1;
    }

    int fb_width = 0;
    int fb_height = 0;
    window->GetFramebufferSize(&fb_width, &fb_height);
    std::unique_ptr<IRenderer> renderer = std::make_unique<PixelRenderer>(fb_width, fb_height);

    GlPresenter presenter;
    if (!presenter.Init()) {
        std::cerr << "Failed to initialize OpenGL presenter\n";
        return 1;
    }

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

        renderer->Clear(ColorRGBA(18, 18, 18, 255));

        int mouse_x = static_cast<int>(input.mouse_x);
        int mouse_y = static_cast<int>(input.mouse_y);
        int pixel_y = fb_height - 1 - mouse_y;

        Color cursor_color = input.IsMouseDown(GLFW_MOUSE_BUTTON_LEFT)
                                 ? ColorRGBA(64, 200, 120, 255)
                                 : ColorRGBA(240, 120, 120, 255);

        for (int dy = -2; dy <= 2; ++dy) {
            for (int dx = -2; dx <= 2; ++dx) {
                renderer->PutPixel(mouse_x + dx, pixel_y + dy, cursor_color);
            }
        }

        presenter.Present(*renderer);
        window->SwapBuffers();
    }

    presenter.Shutdown();
    return 0;
}

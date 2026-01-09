#pragma once

#include "engine/core/IWindow.h"

#include <GLFW/glfw3.h>

class GlfwWindow : public IWindow {
  public:
    GlfwWindow(int width, int height, const char* title);
    ~GlfwWindow();

    bool IsValid() const override { return window_ != nullptr; }
    bool ShouldClose() const override;
    void SetShouldClose(bool value) override;
    void SetFullscreen(bool enabled) override;
    bool IsFullscreen() const override { return is_fullscreen_; }
    void SetVsync(bool enabled) override;
    bool IsVsync() const override { return vsync_enabled_; }
    void PollEvents() override;
    void SwapBuffers() override;
    void GetFramebufferSize(int* width, int* height) const override;
    void* NativeHandle() const override { return window_; }

    InputState& Input() override { return input_; }
    const InputState& Input() const override { return input_; }

  private:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    GLFWwindow* window_ = nullptr;
    InputState input_{};
    bool is_fullscreen_ = false;
    int windowed_x_ = 0;
    int windowed_y_ = 0;
    int windowed_w_ = 800;
    int windowed_h_ = 600;
    bool vsync_enabled_ = true;
};

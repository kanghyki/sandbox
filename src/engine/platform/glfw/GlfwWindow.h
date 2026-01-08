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
    void PollEvents() override;
    void SwapBuffers() override;
    void GetFramebufferSize(int* width, int* height) const override;

    InputState& Input() override { return input_; }
    const InputState& Input() const override { return input_; }

  private:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    GLFWwindow* window_ = nullptr;
    InputState input_{};
};

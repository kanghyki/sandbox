#pragma once

#include <GLFW/glfw3.h>

class ImGuiLayer {
  public:
    bool Init(GLFWwindow* window);
    void Shutdown();
    void BeginFrame();
    void EndFrame();

  private:
    bool initialized_ = false;
};

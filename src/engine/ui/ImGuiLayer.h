#pragma once

#include <GLFW/glfw3.h>

class ImGuiLayer {
  public:
#if defined(SANDBOX_D3D11)
    bool Init(GLFWwindow* window, struct ID3D11Device* device, struct ID3D11DeviceContext* context);
#else
    bool Init(GLFWwindow* window);
#endif
    void Shutdown();
    void BeginFrame();
    void EndFrame();

  private:
    bool initialized_ = false;
};

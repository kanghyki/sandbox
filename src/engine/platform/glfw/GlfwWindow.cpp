#include "engine/platform/glfw/GlfwWindow.h"

#include <iostream>

namespace {
bool g_glfw_initialized = false;
}

GlfwWindow::GlfwWindow(int width, int height, const char* title) {
    if (!g_glfw_initialized) {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW\n";
            return;
        }
        g_glfw_initialized = true;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window_) {
        std::cerr << "Failed to create GLFW window\n";
        return;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetCursorPosCallback(window_, CursorPosCallback);
    glfwSetMouseButtonCallback(window_, MouseButtonCallback);
}

GlfwWindow::~GlfwWindow() {
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    if (g_glfw_initialized) {
        glfwTerminate();
        g_glfw_initialized = false;
    }
}

bool GlfwWindow::ShouldClose() const { return window_ ? glfwWindowShouldClose(window_) : true; }

void GlfwWindow::SetShouldClose(bool value) {
    if (window_) {
        glfwSetWindowShouldClose(window_, value ? GLFW_TRUE : GLFW_FALSE);
    }
}

void GlfwWindow::PollEvents() { glfwPollEvents(); }

void GlfwWindow::SwapBuffers() {
    if (window_) {
        glfwSwapBuffers(window_);
    }
}

void GlfwWindow::GetFramebufferSize(int* width, int* height) const {
    if (window_) {
        glfwGetFramebufferSize(window_, width, height);
    } else {
        if (width) {
            *width = 0;
        }
        if (height) {
            *height = 0;
        }
    }
}

void GlfwWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;
    auto* self = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
    if (!self) {
        return;
    }
    if (key >= 0 && key < static_cast<int>(self->input_.keys.size())) {
        if (action == GLFW_PRESS) {
            self->input_.keys[static_cast<size_t>(key)] = 1;
        } else if (action == GLFW_RELEASE) {
            self->input_.keys[static_cast<size_t>(key)] = 0;
        }
    }
}

void GlfwWindow::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* self = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
    if (!self) {
        return;
    }
    int win_w = 0;
    int win_h = 0;
    int fb_w = 0;
    int fb_h = 0;
    glfwGetWindowSize(window, &win_w, &win_h);
    glfwGetFramebufferSize(window, &fb_w, &fb_h);

    // Convert window-space cursor coordinates to framebuffer pixels (HiDPI safe).
    double scale_x = (win_w > 0) ? static_cast<double>(fb_w) / win_w : 1.0;
    double scale_y = (win_h > 0) ? static_cast<double>(fb_h) / win_h : 1.0;

    self->input_.mouse_x = xpos * scale_x;
    self->input_.mouse_y = ypos * scale_y;
}

void GlfwWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;
    auto* self = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
    if (!self) {
        return;
    }
    if (button >= 0 && button < static_cast<int>(self->input_.mouse_buttons.size())) {
        if (action == GLFW_PRESS) {
            self->input_.mouse_buttons[static_cast<size_t>(button)] = 1;
        } else if (action == GLFW_RELEASE) {
            self->input_.mouse_buttons[static_cast<size_t>(button)] = 0;
        }
    }
}

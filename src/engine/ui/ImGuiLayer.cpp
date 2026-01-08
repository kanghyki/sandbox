#include "engine/ui/ImGuiLayer.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <fstream>
#include <imgui.h>

namespace {
void ApplyEditorStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark(&style);

    style.WindowRounding = 3.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;
    style.PopupRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.94f, 0.96f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.62f, 0.68f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.09f, 0.11f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.06f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.08f, 0.10f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(0.16f, 0.20f, 0.24f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.15f, 0.19f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.23f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.22f, 0.27f, 0.33f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.13f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.06f, 0.08f, 0.10f, 0.75f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.07f, 0.09f, 0.11f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.07f, 0.09f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.26f, 0.32f, 0.38f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.30f, 0.38f, 0.45f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.29f, 0.78f, 0.74f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.29f, 0.78f, 0.74f, 0.85f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.29f, 0.78f, 0.74f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.13f, 0.17f, 0.22f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.29f, 0.78f, 0.74f, 0.75f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.29f, 0.78f, 0.74f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.16f, 0.20f, 0.26f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.29f, 0.78f, 0.74f, 0.55f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.29f, 0.78f, 0.74f, 0.80f);
    colors[ImGuiCol_Separator] = ImVec4(0.16f, 0.20f, 0.24f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.29f, 0.78f, 0.74f, 0.55f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.29f, 0.78f, 0.74f, 0.85f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.78f, 0.74f, 0.55f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.29f, 0.78f, 0.74f, 0.90f);
    colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.13f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.78f, 0.74f, 0.70f);
    colors[ImGuiCol_TabActive] = ImVec4(0.29f, 0.78f, 0.74f, 0.95f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.11f, 0.14f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.16f, 0.20f, 0.26f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.29f, 0.78f, 0.74f, 0.40f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.05f, 0.07f, 0.09f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.29f, 0.78f, 0.74f, 0.90f);
}
} // namespace

bool ImGuiLayer::Init(GLFWwindow* window) {
    if (initialized_) {
        return true;
    }
    if (!window) {
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.IniFilename = "imgui.ini";
    if (io.IniFilename && io.IniFilename[0] != '\0') {
        std::ifstream ini_file(io.IniFilename);
        if (ini_file.good()) {
            ImGui::LoadIniSettingsFromDisk(io.IniFilename);
        }
    }

    ApplyEditorStyle();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    initialized_ = true;
    return true;
}

void ImGuiLayer::Shutdown() {
    if (!initialized_) {
        return;
    }
    ImGuiIO& io = ImGui::GetIO();
    if (io.IniFilename && io.IniFilename[0] != '\0') {
        ImGui::SaveIniSettingsToDisk(io.IniFilename);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    initialized_ = false;
}

void ImGuiLayer::BeginFrame() {
    if (!initialized_) {
        return;
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::EndFrame() {
    if (!initialized_) {
        return;
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup);
    }
}

#include "engine/ui/EditorUi.h"

#include "engine/core/Logger.h"
#include "engine/scene/SceneManager.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <imgui.h>
#include <imgui_internal.h>

namespace {
void DrawViewportImage(unsigned int texture_id) {
    ImVec2 size = ImGui::GetContentRegionAvail();
    if (size.x <= 0.0f || size.y <= 0.0f) {
        return;
    }
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture_id)), size, ImVec2(0, 1),
                 ImVec2(1, 0));
}

void BuildDefaultDockLayout(ImGuiID dockspace_id) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

    ImGuiID dock_main = dockspace_id;
    ImGuiID dock_left =
        ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.22f, nullptr, &dock_main);
    ImGuiID dock_right =
        ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.22f, nullptr, &dock_main);
    ImGuiID dock_bottom =
        ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.26f, nullptr, &dock_main);

    ImGuiID dock_left_top =
        ImGui::DockBuilderSplitNode(dock_left, ImGuiDir_Down, 0.55f, nullptr, &dock_left);
    ImGuiID dock_right_top =
        ImGui::DockBuilderSplitNode(dock_right, ImGuiDir_Down, 0.38f, nullptr, &dock_right);
    ImGuiID dock_right_mid =
        ImGui::DockBuilderSplitNode(dock_right, ImGuiDir_Down, 0.45f, nullptr, &dock_right);

    ImGuiID dock_bottom_right =
        ImGui::DockBuilderSplitNode(dock_bottom, ImGuiDir_Right, 0.45f, nullptr, &dock_bottom);

    ImGui::DockBuilderDockWindow("Scene", dock_left);
    ImGui::DockBuilderDockWindow("Operations", dock_left);
    ImGui::DockBuilderDockWindow("Node Properties", dock_left_top);
    ImGui::DockBuilderDockWindow("Scene for Camera A", dock_main);
    ImGui::DockBuilderDockWindow("Commands", dock_right_top);
    ImGui::DockBuilderDockWindow("Layers", dock_right_mid);
    ImGui::DockBuilderDockWindow("Debug View", dock_right);
    ImGui::DockBuilderDockWindow("Log", dock_bottom);
    ImGui::DockBuilderDockWindow("Viewport Config", dock_bottom_right);
    ImGui::DockBuilderDockWindow("Physics", dock_bottom_right);
    ImGui::DockBuilderDockWindow("Fly Camera", dock_bottom_right);

    ImGui::DockBuilderFinish(dockspace_id);
}
} // namespace

void EditorUi::Draw(unsigned int texture_id, int fb_width, int fb_height, SceneManager& scenes) {
    const ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    viewport_has_mouse_ = false;

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New Scene");
            ImGui::MenuItem("Open...");
            ImGui::MenuItem("Save");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo");
            ImGui::MenuItem("Redo");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Playback")) {
            bool can_play = play_state_ != PlayState::Playing;
            bool can_pause = play_state_ == PlayState::Playing;
            bool can_stop = play_state_ != PlayState::Stopped;
            if (ImGui::MenuItem("Play", nullptr, false, can_play)) {
                play_state_ = PlayState::Playing;
            }
            if (ImGui::MenuItem("Pause", nullptr, false, can_pause)) {
                play_state_ = PlayState::Paused;
            }
            if (ImGui::MenuItem("Stop", nullptr, false, can_stop)) {
                play_state_ = PlayState::Stopped;
                stop_requested_ = true;
            }
            if (ImGui::MenuItem("Step", nullptr, false, play_state_ == PlayState::Paused)) {
                step_requested_ = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            if (ImGui::MenuItem("Reset Layout")) {
                request_layout_reset_ = true;
            }
            ImGui::Separator();
            bool was_scene = show_scene_;
            if (ImGui::MenuItem("Scene", nullptr, &show_scene_)) {
                if (!was_scene && show_scene_) {
                    request_show_scene_ = true;
                }
            }
            ImGui::MenuItem("Operations", nullptr, &show_operations_);
            ImGui::MenuItem("Node Properties", nullptr, &show_node_properties_);
            ImGui::MenuItem("Viewport", nullptr, &show_viewport_);
            ImGui::MenuItem("Commands", nullptr, &show_commands_);
            ImGui::MenuItem("Layers", nullptr, &show_layers_);
            ImGui::MenuItem("Debug View", nullptr, &show_debug_view_);
            ImGui::MenuItem("Log", nullptr, &show_log_);
            ImGui::MenuItem("Viewport Config", nullptr, &show_viewport_config_);
            ImGui::MenuItem("Physics", nullptr, &show_physics_);
            ImGui::MenuItem("Fly Camera", nullptr, &show_fly_camera_);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dock_flags);

    if (request_layout_reset_ || !dock_built_) {
        bool should_build = request_layout_reset_;
        if (!should_build) {
            ImGuiIO& io = ImGui::GetIO();
            const char* ini_path = io.IniFilename;
            if (ini_path && ini_path[0] != '\0') {
                std::ifstream ini_file(ini_path);
                should_build = !ini_file.good();
            } else {
                should_build = true;
            }
        }
        if (should_build) {
            BuildDefaultDockLayout(dockspace_id);
        }
        dock_built_ = true;
        request_layout_reset_ = false;
    }
    ImGui::End();

    if (show_scene_) {
        if (request_show_scene_) {
            ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_Always);
            ImGui::SetNextWindowFocus();
            request_show_scene_ = false;
        }
        if (ImGui::Begin("Scene", &show_scene_)) {
            ImGui::Text("Scenes");
            ImGui::Separator();
            size_t count = scenes.SceneCount();
            for (size_t i = 0; i < count; ++i) {
                const IScene* scene = scenes.GetScene(i);
                const char* name = scene ? scene->Name() : "Unknown";
                bool selected = i == scenes.ActiveIndex();
                if (ImGui::Selectable(name, selected)) {
                    scenes.SetActiveIndex(i);
                    Logger::Info(std::string("Switched to scene: ") + name);
                }
            }
            if (count == 0) {
                ImGui::TextDisabled("No scenes loaded.");
            }
            if (IScene* active = scenes.ActiveScene()) {
                ImGui::Separator();
                active->DrawSceneGui();
            }
        }
        ImGui::End();
    }

    if (show_operations_) {
        if (ImGui::Begin("Operations", &show_operations_)) {
            ImGui::Button("Undo");
            ImGui::Button("Redo");
            ImGui::Separator();
            ImGui::Button("Attach");
            ImGui::Button("Detach");
            ImGui::Button("Triangulate");
        }
        ImGui::End();
    }

    if (show_node_properties_) {
        if (ImGui::Begin("Node Properties", &show_node_properties_)) {
            if (IScene* active = scenes.ActiveScene()) {
                active->DrawInspectorGui();
            } else {
                ImGui::TextDisabled("No active scene.");
            }
        }
        ImGui::End();
    }

    if (show_viewport_) {
        if (ImGui::Begin("Scene for Camera A", &show_viewport_)) {
            ImVec2 avail = ImGui::GetContentRegionAvail();
            ImVec2 image_pos = ImGui::GetCursorScreenPos();
            DrawViewportImage(texture_id);

            ImGuiIO& io = ImGui::GetIO();
            ImVec2 mouse = io.MousePos;
            bool inside = mouse.x >= image_pos.x && mouse.y >= image_pos.y &&
                          mouse.x <= image_pos.x + avail.x && mouse.y <= image_pos.y + avail.y;
            if (inside && avail.x > 0.0f && avail.y > 0.0f && fb_width > 0 && fb_height > 0) {
                float local_x = mouse.x - image_pos.x;
                float local_y = mouse.y - image_pos.y;
                int px = static_cast<int>((local_x / avail.x) * fb_width);
                int py = static_cast<int>((local_y / avail.y) * fb_height);
                px = std::clamp(px, 0, fb_width - 1);
                py = std::clamp(py, 0, fb_height - 1);
                viewport_has_mouse_ = true;
                viewport_mouse_x_ = px;
                viewport_mouse_y_ = fb_height - 1 - py;
            }
        }
        ImGui::End();
    }

    if (show_commands_) {
        if (ImGui::Begin("Commands", &show_commands_)) {
            ImGui::Text("Active command: none");
            ImGui::Separator();
            ImGui::BulletText("Selection");
            ImGui::BulletText("Brush");
            ImGui::BulletText("Transform");
        }
        ImGui::End();
    }

    if (show_layers_) {
        if (ImGui::Begin("Layers", &show_layers_)) {
            ImGui::Selectable("content");
            ImGui::Selectable("controller");
            ImGui::Selectable("tool");
            ImGui::Selectable("brush");
        }
        ImGui::End();
    }

    if (show_debug_view_) {
        if (ImGui::Begin("Debug View", &show_debug_view_)) {
            ImGui::Text("Viewport helpers");
            ImGui::Button("Toggle Gizmos");
            ImGui::Button("Wireframe");
        }
        ImGui::End();
    }

    if (show_log_) {
        if (ImGui::Begin("Log", &show_log_)) {
            if (ImGui::SmallButton("Clear")) {
                Logger::Clear();
            }
            ImGui::Separator();
            for (const LogEntry& entry : Logger::Entries()) {
                const char* label = "INFO";
                ImVec4 color = ImVec4(0.8f, 0.85f, 0.9f, 1.0f);
                if (entry.level == LogLevel::Warn) {
                    label = "WARN";
                    color = ImVec4(0.95f, 0.75f, 0.2f, 1.0f);
                } else if (entry.level == LogLevel::Error) {
                    label = "ERROR";
                    color = ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
                }
                std::string line = std::string("[") + label + "] " + entry.text;
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(line.c_str());
                ImGui::PopStyleColor();
            }
        }
        ImGui::End();
    }

    if (show_viewport_config_) {
        if (ImGui::Begin("Viewport Config", &show_viewport_config_)) {
            ImGui::Text("Clear Color");
            ImGui::ColorEdit3("##clear", clear_color_);
        }
        ImGui::End();
    }

    if (show_physics_) {
        if (ImGui::Begin("Physics", &show_physics_)) {
            ImGui::Checkbox("Enable", &physics_enabled_);
            ImGui::SliderFloat("Gravity", &gravity_, -20.0f, 0.0f);
        }
        ImGui::End();
    }

    if (show_fly_camera_) {
        if (ImGui::Begin("Fly Camera", &show_fly_camera_)) {
            ImGui::SliderFloat("Sensitivity", &fly_sensitivity_, 0.0f, 2.0f);
            ImGui::SliderFloat("Speed", &fly_speed_, 0.0f, 10.0f);
        }
        ImGui::End();
    }
}

bool EditorUi::ConsumeStepRequested() {
    if (!step_requested_) {
        return false;
    }
    step_requested_ = false;
    return true;
}

bool EditorUi::ConsumeStopRequested() {
    if (!stop_requested_) {
        return false;
    }
    stop_requested_ = false;
    return true;
}

bool EditorUi::GetViewportMousePixel(int* out_x, int* out_y) const {
    if (!viewport_has_mouse_ || !out_x || !out_y) {
        return false;
    }
    *out_x = viewport_mouse_x_;
    *out_y = viewport_mouse_y_;
    return true;
}

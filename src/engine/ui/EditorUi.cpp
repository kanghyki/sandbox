#include "engine/ui/EditorUi.h"

#include "engine/core/Logger.h"
#include "engine/scene/SceneManager.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <imgui.h>
#include <imgui_internal.h>

namespace {
void DrawViewportImage(unsigned int texture_id, int fb_width, int fb_height, ImVec2* out_pos,
                       ImVec2* out_size) {
    ImVec2 size = ImGui::GetContentRegionAvail();
    if (size.x <= 0.0f || size.y <= 0.0f) {
        return;
    }
    float scale_x = (fb_width > 0) ? (size.x / static_cast<float>(fb_width)) : 1.0f;
    float scale_y = (fb_height > 0) ? (size.y / static_cast<float>(fb_height)) : 1.0f;
    float scale = std::min(scale_x, scale_y);
    ImVec2 image_size(static_cast<float>(fb_width) * scale, static_cast<float>(fb_height) * scale);

    ImVec2 cursor = ImGui::GetCursorPos();
    ImVec2 region = ImGui::GetContentRegionAvail();
    ImVec2 padding((region.x - image_size.x) * 0.5f, (region.y - image_size.y) * 0.5f);
    if (padding.x < 0.0f) {
        padding.x = 0.0f;
    }
    if (padding.y < 0.0f) {
        padding.y = 0.0f;
    }
    ImGui::SetCursorPos(ImVec2(cursor.x + padding.x, cursor.y + padding.y));
    ImVec2 image_pos = ImGui::GetCursorScreenPos();
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture_id)), image_size,
                 ImVec2(0, 1), ImVec2(1, 0));
    if (out_pos) {
        *out_pos = image_pos;
    }
    if (out_size) {
        *out_size = image_size;
    }
}

void BuildDefaultDockLayout(ImGuiID dockspace_id) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

    ImGuiID dock_main = dockspace_id;
    ImGuiID dock_left =
        ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.22f, nullptr, &dock_main);
    ImGuiID dock_bottom =
        ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.26f, nullptr, &dock_main);

    ImGuiID dock_left_top =
        ImGui::DockBuilderSplitNode(dock_left, ImGuiDir_Down, 0.55f, nullptr, &dock_left);

    ImGuiID dock_bottom_right =
        ImGui::DockBuilderSplitNode(dock_bottom, ImGuiDir_Right, 0.45f, nullptr, &dock_bottom);

    ImGui::DockBuilderDockWindow("Scene", dock_left);
    ImGui::DockBuilderDockWindow("Node Properties", dock_left_top);
    ImGui::DockBuilderDockWindow("Scene for Camera A", dock_main);
    ImGui::DockBuilderDockWindow("Log", dock_bottom);
    ImGui::DockBuilderDockWindow("Render Settings", dock_bottom_right);

    ImGui::DockBuilderFinish(dockspace_id);
}
} // namespace

void EditorUi::Draw(unsigned int texture_id, int fb_width, int fb_height, int win_width,
                    int win_height, SceneManager& scenes) {
    const ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    viewport_has_mouse_ = false;

    if (focus_viewport_) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGuiWindowFlags focus_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus |
                                       ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("ViewportFocus", nullptr, focus_flags);
        ImGui::PopStyleVar(3);

        ImVec2 image_pos{};
        ImVec2 image_size{};
        DrawViewportImage(texture_id, fb_width, fb_height, &image_pos, &image_size);

        if (show_fps_overlay_) {
            ImGuiIO& io = ImGui::GetIO();
            float fps = io.Framerate;
            char text[64] = {};
            std::snprintf(text, sizeof(text), "%.1f FPS", fps);

            ImVec2 pad(6.0f, 4.0f);
            ImVec2 text_size = ImGui::CalcTextSize(text);
            ImVec2 pos(image_pos.x + image_size.x - text_size.x - pad.x * 2.0f - 8.0f,
                       image_pos.y + 8.0f);
            ImGui::SetCursorScreenPos(pos);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.11f, 0.14f, 0.85f));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
            ImGui::BeginChild("##fps_overlay_focus",
                              ImVec2(text_size.x + pad.x * 2.0f, text_size.y + pad.y * 2.0f), false,
                              ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav);
            ImGui::SetCursorPos(pad);
            ImGui::TextUnformatted(text);
            ImGui::EndChild();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }

        ImGuiIO& io = ImGui::GetIO();
        ImVec2 mouse = io.MousePos;
        bool inside = mouse.x >= image_pos.x && mouse.y >= image_pos.y &&
                      mouse.x <= image_pos.x + image_size.x &&
                      mouse.y <= image_pos.y + image_size.y;
        if (inside && image_size.x > 0.0f && image_size.y > 0.0f && fb_width > 0 && fb_height > 0) {
            float local_x = mouse.x - image_pos.x;
            float local_y = mouse.y - image_pos.y;
            int px = static_cast<int>((local_x / image_size.x) * fb_width);
            int py = static_cast<int>((local_y / image_size.y) * fb_height);
            px = std::clamp(px, 0, fb_width - 1);
            py = std::clamp(py, 0, fb_height - 1);
            viewport_has_mouse_ = true;
            viewport_mouse_x_ = px;
            viewport_mouse_y_ = fb_height - 1 - py;
        }

        ImGui::End();

        ImGuiWindowFlags overlay_flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav;
        ImGui::SetNextWindowPos(
            ImVec2(viewport->Pos.x + viewport->Size.x - 12.0f, viewport->Pos.y + 12.0f),
            ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowBgAlpha(0.35f);
        ImGui::Begin("ViewportFocusControls", nullptr, overlay_flags);
        if (ImGui::Button("Reduce")) {
            focus_viewport_ = false;
        }
        ImGui::End();
        return;
    }

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
            ImGui::MenuItem("Node Properties", nullptr, &show_node_properties_);
            ImGui::MenuItem("Viewport", nullptr, &show_viewport_);
            ImGui::MenuItem("Log", nullptr, &show_log_);
            ImGui::MenuItem("Viewport Config", nullptr, &show_viewport_config_);
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
            if (ImGui::Button("Focus View")) {
                focus_viewport_ = true;
            }
            ImGui::Separator();
            ImVec2 avail = ImGui::GetContentRegionAvail();
            ImVec2 image_pos{};
            ImVec2 image_size{};
            DrawViewportImage(texture_id, fb_width, fb_height, &image_pos, &image_size);

            if (show_fps_overlay_) {
                ImGuiIO& io = ImGui::GetIO();
                float fps = io.Framerate;
                char text[64] = {};
                std::snprintf(text, sizeof(text), "%.1f FPS", fps);

                ImVec2 pad(6.0f, 4.0f);
                ImVec2 text_size = ImGui::CalcTextSize(text);
                ImVec2 pos(image_pos.x + image_size.x - text_size.x - pad.x * 2.0f - 8.0f,
                           image_pos.y + 8.0f);
                ImGui::SetCursorScreenPos(pos);
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.11f, 0.14f, 0.85f));
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
                ImGui::BeginChild("##fps_overlay",
                                  ImVec2(text_size.x + pad.x * 2.0f, text_size.y + pad.y * 2.0f),
                                  false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav);
                ImGui::SetCursorPos(pad);
                ImGui::TextUnformatted(text);
                ImGui::EndChild();
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
            }

            ImGuiIO& io = ImGui::GetIO();
            ImVec2 mouse = io.MousePos;
            bool inside = mouse.x >= image_pos.x && mouse.y >= image_pos.y &&
                          mouse.x <= image_pos.x + image_size.x &&
                          mouse.y <= image_pos.y + image_size.y;
            if (inside && image_size.x > 0.0f && image_size.y > 0.0f && fb_width > 0 &&
                fb_height > 0) {
                float local_x = mouse.x - image_pos.x;
                float local_y = mouse.y - image_pos.y;
                int px = static_cast<int>((local_x / image_size.x) * fb_width);
                int py = static_cast<int>((local_y / image_size.y) * fb_height);
                px = std::clamp(px, 0, fb_width - 1);
                py = std::clamp(py, 0, fb_height - 1);
                viewport_has_mouse_ = true;
                viewport_mouse_x_ = px;
                viewport_mouse_y_ = fb_height - 1 - py;
            }
        }
        ImGui::End();
    }

    if (show_log_) {
        if (ImGui::Begin("Log", &show_log_)) {
            if (ImGui::SmallButton("Clear")) {
                Logger::Clear();
            }
            ImGui::Separator();
            bool at_bottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 1.0f;
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
            if (at_bottom) {
                ImGui::SetScrollHereY(1.0f);
            }
        }
        ImGui::End();
    }

    if (show_viewport_config_) {
        if (ImGui::Begin("Render Settings", &show_viewport_config_)) {
            ImGui::Text("Render Target");
            const char* presets[] = {"960x540",   "1280x720",  "1600x900",
                                     "1920x1080", "2560x1440", "Custom"};
            const int preset_count = static_cast<int>(sizeof(presets) / sizeof(presets[0]));
            if (ImGui::Combo("Preset", &viewport_resolution_index_, presets, preset_count)) {
                switch (viewport_resolution_index_) {
                case 0:
                    viewport_target_width_ = 960;
                    viewport_target_height_ = 540;
                    break;
                case 1:
                    viewport_target_width_ = 1280;
                    viewport_target_height_ = 720;
                    break;
                case 2:
                    viewport_target_width_ = 1600;
                    viewport_target_height_ = 900;
                    break;
                case 3:
                    viewport_target_width_ = 1920;
                    viewport_target_height_ = 1080;
                    break;
                case 4:
                    viewport_target_width_ = 2560;
                    viewport_target_height_ = 1440;
                    break;
                default:
                    break;
                }
            }
            const bool is_custom = viewport_resolution_index_ == preset_count - 1;
            if (is_custom) {
                ImGui::PushItemWidth(120.0f);
                ImGui::InputInt("W", &viewport_target_width_);
                ImGui::SameLine();
                ImGui::InputInt("H", &viewport_target_height_);
                ImGui::PopItemWidth();
            }
            viewport_target_width_ = std::max(1, viewport_target_width_);
            viewport_target_height_ = std::max(1, viewport_target_height_);
            ImGui::Text("Active: %d x %d", viewport_target_width_, viewport_target_height_);
            ImGui::Separator();

            ImGui::Text("Window: %d x %d", win_width, win_height);
            ImGui::Text("Framebuffer: %d x %d", fb_width, fb_height);
            float scale_x = win_width > 0 ? static_cast<float>(fb_width) / win_width : 0.0f;
            float scale_y = win_height > 0 ? static_cast<float>(fb_height) / win_height : 0.0f;
            ImGui::Text("DPI Scale: %.2f x %.2f", scale_x, scale_y);
            ImGui::ColorEdit4("##clear", clear_color_, ImGuiColorEditFlags_AlphaBar);
            ImGui::Checkbox("FPS Overlay", &show_fps_overlay_);
            ImGui::Separator();

            ImGui::Text("System");
            ImGui::Checkbox("VSync", &vsync_enabled_);
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

#pragma once

class EditorUi {
  public:
    enum class PlayState {
        Stopped,
        Playing,
        Paused,
    };

    void Draw(unsigned int texture_id, int fb_width, int fb_height, int win_width, int win_height,
              class SceneManager& scenes);
    const float* ClearColor() const { return clear_color_; }
    bool GetViewportMousePixel(int* out_x, int* out_y) const;
    bool IsViewportHovered() const { return viewport_has_mouse_; }
    int ViewportTargetWidth() const { return viewport_target_width_; }
    int ViewportTargetHeight() const { return viewport_target_height_; }
    PlayState GetPlayState() const { return play_state_; }
    bool ConsumeStepRequested();
    bool ConsumeStopRequested();
    bool VsyncEnabled() const { return vsync_enabled_; }
    bool ShowFpsOverlay() const { return show_fps_overlay_; }
    void SetFocusViewport(bool enabled) { focus_viewport_ = enabled; }

  private:
    bool dock_built_ = false;
    bool request_layout_reset_ = false;
    float clear_color_[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    bool vsync_enabled_ = true;
    bool show_fps_overlay_ = true;
    PlayState play_state_ = PlayState::Playing;
    bool step_requested_ = false;
    bool stop_requested_ = false;
    bool show_scene_ = true;
    bool show_node_properties_ = true;
    bool show_viewport_ = true;
    bool show_log_ = true;
    bool show_viewport_config_ = true;
    bool request_show_scene_ = false;
    bool viewport_has_mouse_ = false;
    int viewport_mouse_x_ = 0;
    int viewport_mouse_y_ = 0;
    int viewport_target_width_ = 960;
    int viewport_target_height_ = 540;
    int viewport_resolution_index_ = 5;
    bool focus_viewport_ = false;
};

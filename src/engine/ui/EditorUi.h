#pragma once

class EditorUi {
  public:
    enum class PlayState {
        Stopped,
        Playing,
        Paused,
    };

    void Draw(unsigned int texture_id, int fb_width, int fb_height, class SceneManager& scenes);
    const float* ClearColor() const { return clear_color_; }
    bool GetViewportMousePixel(int* out_x, int* out_y) const;
    bool IsViewportHovered() const { return viewport_has_mouse_; }
    PlayState GetPlayState() const { return play_state_; }
    bool ConsumeStepRequested();
    bool ConsumeStopRequested();

  private:
    bool dock_built_ = false;
    bool request_layout_reset_ = false;
    float projection_fov_ = 60.0f;
    float light_color_[3] = {1.0f, 0.9f, 0.7f};
    float light_intensity_ = 5.0f;
    float transform_pos_[3] = {0.0f, 1.2f, 0.0f};
    float transform_rot_[3] = {0.0f, 35.0f, 0.0f};
    float transform_scale_[3] = {1.0f, 1.0f, 1.0f};
    float clear_color_[3] = {0.06f, 0.12f, 0.14f};
    bool physics_enabled_ = true;
    float gravity_ = -9.8f;
    float fly_sensitivity_ = 0.3f;
    float fly_speed_ = 2.5f;
    PlayState play_state_ = PlayState::Playing;
    bool step_requested_ = false;
    bool stop_requested_ = false;
    bool show_scene_ = true;
    bool show_operations_ = false;
    bool show_node_properties_ = true;
    bool show_viewport_ = true;
    bool show_commands_ = false;
    bool show_layers_ = false;
    bool show_debug_view_ = false;
    bool show_log_ = true;
    bool show_viewport_config_ = true;
    bool show_physics_ = false;
    bool show_fly_camera_ = false;
    bool viewport_has_mouse_ = false;
    int viewport_mouse_x_ = 0;
    int viewport_mouse_y_ = 0;
};

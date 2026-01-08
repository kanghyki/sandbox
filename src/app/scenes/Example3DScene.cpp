#include "app/scenes/Example3DScene.h"

#include "engine/core/Color.h"

#include <GLFW/glfw3.h>
#include <cmath>
#include <imgui.h>

namespace {
struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

Vec3 Add(const Vec3& a, const Vec3& b) { return Vec3{a.x + b.x, a.y + b.y, a.z + b.z}; }

Vec3 Sub(const Vec3& a, const Vec3& b) { return Vec3{a.x - b.x, a.y - b.y, a.z - b.z}; }

Vec3 Scale(const Vec3& v, float s) { return Vec3{v.x * s, v.y * s, v.z * s}; }

float Dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

Vec3 Cross(const Vec3& a, const Vec3& b) {
    return Vec3{a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

float Length(const Vec3& v) { return std::sqrt(Dot(v, v)); }

Vec3 Normalize(const Vec3& v) {
    float len = Length(v);
    if (len <= 0.0f) {
        return v;
    }
    return Scale(v, 1.0f / len);
}

Vec3 RotateYawPitchRoll(const Vec3& v, float yaw, float pitch, float roll) {
    float cy = std::cos(yaw);
    float sy = std::sin(yaw);
    float cx = std::cos(pitch);
    float sx = std::sin(pitch);
    float cz = std::cos(roll);
    float sz = std::sin(roll);

    Vec3 vy{cy * v.x + sy * v.z, v.y, -sy * v.x + cy * v.z};
    Vec3 vp{vy.x, cx * vy.y - sx * vy.z, sx * vy.y + cx * vy.z};
    Vec3 vr{cz * vp.x - sz * vp.y, sz * vp.x + cz * vp.y, vp.z};
    return vr;
}

void DrawLine(IRenderer& renderer, int x0, int y0, int x1, int y1, Color color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int x = x0;
    int y = y0;
    while (true) {
        renderer.PutPixel(x, y, color);
        if (x == x1 && y == y1) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

bool ProjectPoint(const Vec3& world, const Vec3& cam_pos, float yaw, float pitch, float fov_rad,
                  float aspect, float near_plane, int width, int height, Vec2* out) {
    Vec3 rel = Sub(world, cam_pos);
    Vec3 view = RotateYawPitchRoll(rel, -yaw, -pitch, 0.0f);
    if (view.z <= near_plane) {
        return false;
    }

    float f = 1.0f / std::tan(fov_rad * 0.5f);
    float x_ndc = (view.x * f / aspect) / view.z;
    float y_ndc = (view.y * f) / view.z;

    out->x = (x_ndc * 0.5f + 0.5f) * static_cast<float>(width);
    out->y = (1.0f - (y_ndc * 0.5f + 0.5f)) * static_cast<float>(height);
    return true;
}
} // namespace

Example3DScene::Example3DScene() {
    Node camera;
    camera.name = "Camera";
    camera.is_camera = true;
    camera.position[2] = -4.0f;
    nodes_.push_back(camera);

    Node cube;
    cube.name = "Cube";
    cube.position[1] = 0.5f;
    cube.shape = Node::Shape::Cube;
    nodes_.push_back(cube);

    Node pyramid;
    pyramid.name = "Pyramid";
    pyramid.position[1] = 0.4f;
    pyramid.shape = Node::Shape::Pyramid;
    nodes_.push_back(pyramid);
}

void Example3DScene::Reset() {
    if (nodes_.empty()) {
        return;
    }
    nodes_[0].position[0] = 0.0f;
    nodes_[0].position[1] = 0.0f;
    nodes_[0].position[2] = -4.0f;
    nodes_[0].rotation[0] = 0.0f;
    nodes_[0].rotation[1] = 0.0f;
    nodes_[0].rotation[2] = 0.0f;
    time_ = 0.0f;
}

void Example3DScene::Update(const FrameContext& context) {
    time_ += context.dt;
    if (nodes_.empty() || !context.input) {
        return;
    }
    Node& camera = nodes_[0];
    const InputState& input = *context.input;

    bool right_mouse = input.IsMouseDown(GLFW_MOUSE_BUTTON_RIGHT);
    if (right_mouse && context.viewport_hovered) {
        if (!has_last_mouse_) {
            last_mouse_x_ = input.mouse_x;
            last_mouse_y_ = input.mouse_y;
            has_last_mouse_ = true;
        } else {
            double dx = input.mouse_x - last_mouse_x_;
            double dy = input.mouse_y - last_mouse_y_;
            last_mouse_x_ = input.mouse_x;
            last_mouse_y_ = input.mouse_y;
            camera.rotation[1] += static_cast<float>(dx) * look_sensitivity_;
            camera.rotation[0] -= static_cast<float>(dy) * look_sensitivity_;
            camera.rotation[0] = std::clamp(camera.rotation[0], -1.5f, 1.5f);
        }
    } else {
        has_last_mouse_ = false;
    }

    float speed = move_speed_;
    if (input.IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        speed *= 2.0f;
    }

    float yaw = camera.rotation[1];
    float pitch = camera.rotation[0];
    Vec3 forward{std::cos(pitch) * std::sin(yaw), std::sin(pitch), std::cos(pitch) * std::cos(yaw)};
    Vec3 right{std::cos(yaw), 0.0f, -std::sin(yaw)};
    Vec3 up{0.0f, 1.0f, 0.0f};

    Vec3 move{};
    if (input.IsKeyDown(GLFW_KEY_W)) {
        move = Add(move, forward);
    }
    if (input.IsKeyDown(GLFW_KEY_S)) {
        move = Sub(move, forward);
    }
    if (input.IsKeyDown(GLFW_KEY_D)) {
        move = Add(move, right);
    }
    if (input.IsKeyDown(GLFW_KEY_A)) {
        move = Sub(move, right);
    }
    if (input.IsKeyDown(GLFW_KEY_E)) {
        move = Add(move, up);
    }
    if (input.IsKeyDown(GLFW_KEY_Q)) {
        move = Sub(move, up);
    }

    if (Length(move) > 0.0f) {
        move = Normalize(move);
        move = Scale(move, speed * context.dt);
        camera.position[0] += move.x;
        camera.position[1] += move.y;
        camera.position[2] += move.z;
    }

    if (animate_) {
        for (size_t i = 1; i < nodes_.size(); ++i) {
            Node& node = nodes_[i];
            node.rotation[1] += spin_speed_ * context.dt;
            if (node.shape == Node::Shape::Pyramid) {
                node.position[0] = std::cos(time_ * orbit_speed_) * orbit_radius_;
                node.position[2] = std::sin(time_ * orbit_speed_) * orbit_radius_;
            }
        }
    }
}

void Example3DScene::Render(IRenderer& renderer) {
    int w = renderer.Width();
    int h = renderer.Height();
    if (w <= 0 || h <= 0 || nodes_.empty()) {
        return;
    }

    const Node& camera = nodes_[0];
    Vec3 cam_pos{camera.position[0], camera.position[1], camera.position[2]};
    float yaw = camera.rotation[1];
    float pitch = camera.rotation[0];
    float fov_rad = fov_degrees_ * 3.1415926f / 180.0f;
    float aspect = static_cast<float>(w) / static_cast<float>(h);

    const Vec3 cube_vertices[] = {
        {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f},
        {-0.5f, -0.5f, 0.5f},  {0.5f, -0.5f, 0.5f},  {0.5f, 0.5f, 0.5f},  {-0.5f, 0.5f, 0.5f},
    };
    const int cube_edges[][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
        {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7},
    };
    const Vec3 pyramid_vertices[] = {
        {-0.6f, -0.5f, -0.6f}, {0.6f, -0.5f, -0.6f}, {0.6f, -0.5f, 0.6f},
        {-0.6f, -0.5f, 0.6f},  {0.0f, 0.6f, 0.0f},
    };
    const int pyramid_edges[][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {0, 4}, {1, 4}, {2, 4}, {3, 4},
    };

    for (size_t n = 1; n < nodes_.size(); ++n) {
        const Node& node = nodes_[n];
        Vec3 pos{node.position[0], node.position[1], node.position[2]};
        Vec3 rot{node.rotation[0], node.rotation[1], node.rotation[2]};
        Vec3 scale{node.scale[0], node.scale[1], node.scale[2]};
        Color edge_color = ColorRGBA(120, 200, 190, 255);

        if (node.shape == Node::Shape::Cube) {
            Vec2 projected[8];
            bool visible[8];
            for (int i = 0; i < 8; ++i) {
                Vec3 local = {cube_vertices[i].x * scale.x, cube_vertices[i].y * scale.y,
                              cube_vertices[i].z * scale.z};
                Vec3 world = RotateYawPitchRoll(local, rot.y, rot.x, rot.z);
                world = Add(world, pos);
                visible[i] = ProjectPoint(world, cam_pos, yaw, pitch, fov_rad, aspect, near_plane_,
                                          w, h, &projected[i]);
            }
            for (const auto& edge : cube_edges) {
                int a = edge[0];
                int b = edge[1];
                if (!visible[a] || !visible[b]) {
                    continue;
                }
                DrawLine(renderer, static_cast<int>(projected[a].x),
                         static_cast<int>(projected[a].y), static_cast<int>(projected[b].x),
                         static_cast<int>(projected[b].y), edge_color);
            }
        } else {
            Vec2 projected[5];
            bool visible[5];
            for (int i = 0; i < 5; ++i) {
                Vec3 local = {pyramid_vertices[i].x * scale.x, pyramid_vertices[i].y * scale.y,
                              pyramid_vertices[i].z * scale.z};
                Vec3 world = RotateYawPitchRoll(local, rot.y, rot.x, rot.z);
                world = Add(world, pos);
                visible[i] = ProjectPoint(world, cam_pos, yaw, pitch, fov_rad, aspect, near_plane_,
                                          w, h, &projected[i]);
            }
            for (const auto& edge : pyramid_edges) {
                int a = edge[0];
                int b = edge[1];
                if (!visible[a] || !visible[b]) {
                    continue;
                }
                DrawLine(renderer, static_cast<int>(projected[a].x),
                         static_cast<int>(projected[a].y), static_cast<int>(projected[b].x),
                         static_cast<int>(projected[b].y), edge_color);
            }
        }
    }
}

void Example3DScene::DrawSceneGui() {
    ImGui::Text("Nodes");
    ImGui::Checkbox("Animate", &animate_);
    ImGui::SameLine();
    ImGui::DragFloat("Spin", &spin_speed_, 0.05f, 0.0f, 5.0f);
    ImGui::Separator();
    for (size_t i = 0; i < nodes_.size(); ++i) {
        bool selected = i == selected_index_;
        if (ImGui::Selectable(nodes_[i].name.c_str(), selected)) {
            selected_index_ = i;
        }
    }
}

void Example3DScene::DrawInspectorGui() {
    if (nodes_.empty() || selected_index_ >= nodes_.size()) {
        ImGui::TextDisabled("No selection.");
        return;
    }

    Node& node = nodes_[selected_index_];
    ImGui::Text("%s", node.name.c_str());
    ImGui::Separator();
    ImGui::DragFloat3("Position", node.position, 0.05f);

    if (node.is_camera) {
        ImGui::DragFloat2("Yaw/Pitch", &node.rotation[1], 0.01f);
        ImGui::SliderFloat("FOV", &fov_degrees_, 30.0f, 120.0f);
        ImGui::SliderFloat("Near", &near_plane_, 0.01f, 1.0f);
        ImGui::SliderFloat("Far", &far_plane_, 10.0f, 200.0f);
    } else {
        ImGui::DragFloat3("Rotation", node.rotation, 0.02f);
        ImGui::DragFloat3("Scale", node.scale, 0.02f, 0.1f, 5.0f);
        ImGui::Text("Shape: %s", node.shape == Node::Shape::Cube ? "Cube" : "Pyramid");
    }
}

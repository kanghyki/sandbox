#pragma once

#include <array>
#include <cstdint>

struct InputState {
    std::array<uint8_t, 512> keys{};
    std::array<uint8_t, 16> mouse_buttons{};
    double mouse_x = 0.0;
    double mouse_y = 0.0;

    bool IsKeyDown(int key) const {
        if (key < 0 || key >= static_cast<int>(keys.size())) {
            return false;
        }
        return keys[static_cast<size_t>(key)] != 0;
    }

    bool IsMouseDown(int button) const {
        if (button < 0 || button >= static_cast<int>(mouse_buttons.size())) {
            return false;
        }
        return mouse_buttons[static_cast<size_t>(button)] != 0;
    }
};

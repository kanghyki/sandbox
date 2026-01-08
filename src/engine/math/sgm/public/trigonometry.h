#ifndef INCLUDED_TRIGONOMETRY_H
#define INCLUDED_TRIGONOMETRY_H

namespace sgm {
inline constexpr float kPi = 3.14159265358979323846f;
inline constexpr float kInvPi = 0.31830988618f;

inline float radians(float degrees) { return degrees * kPi / 180.0f; }

inline float degrees(float radians) { return radians * 180.0f * kInvPi; }
} // namespace sgm

#endif

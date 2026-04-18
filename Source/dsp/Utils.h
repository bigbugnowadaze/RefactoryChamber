#pragma once

#include <algorithm>
#include <array>
#include <cmath>

namespace rc::dsp
{
inline float clamp01(float v) noexcept { return std::clamp(v, 0.0f, 1.0f); }

inline float fastSat(float z, float cubic = 0.12f) noexcept
{
    const float t = std::tanh(z);
    const float c = cubic * z * z * z / (1.0f + std::abs(z));
    return std::clamp(t + c, -1.2f, 1.2f);
}
}

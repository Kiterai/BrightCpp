#pragma once

#include <brightcpp/common.hpp>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

struct alignas(8) vec2 {
    float v[2];
};

struct alignas(16) vec3 {
    float v[3];
};

struct alignas(16) vec4 {
    float v[4];
};

struct alignas(16) mat3 {
    vec3 m[3];
};

inline constexpr mat3 operator*(mat3 a, mat3 b) {
    mat3 m{};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                m.m[i].v[j] += a.m[i].v[k] * b.m[k].v[j];
    return m;
}

struct alignas(16) mat4 {
    vec4 m[4];
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

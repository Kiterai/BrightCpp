#pragma once

#include <brightcpp/internal/linear_algebra.hpp>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

struct render_rect_info {
    vec2 pos, size, apos, clip_pos, clip_size;
    vec4 color;
    float theta;
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

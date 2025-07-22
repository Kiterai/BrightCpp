#include "../linear_algebra.hpp"

BRIGHTCPP_START

namespace internal {

struct renderer2d_vertex {
    vec2 pos;
    vec2 uv;
};

struct renderer2d_uniform {
    mat3 draw_matrix;
    vec2 screen_size;
    vec2 tex_clip_pos;
    vec2 tex_clip_size;
    vec4 color;
};

} // namespace internal

BRIGHTCPP_END

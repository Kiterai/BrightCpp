#pragma once

#include "../linear_algebra.hpp"
#include <brightcpp/image.hpp>
#include <brightcpp/renderer2d.hpp>
#include <memory>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

struct render_texture_info {
    vec2 pos, size, apos, clip_pos, clip_size;
    vec4 color;
    float theta;
};

class renderer2d_backend {
  public:
    virtual void draw_texture(handle_holder<image_impl> image, render_texture_info &info) = 0;
};

class renderer2d_factory_backend {
  public:
    virtual handle_holder<renderer2d>::handle_value_t make(render_target rt) = 0;
    virtual renderer2d_backend &get(handle_holder<renderer2d> handle) = 0;
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

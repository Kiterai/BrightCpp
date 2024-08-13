#pragma once

#include "../linear_algebra.hpp"
#include <brightcpp/image_decl.hpp>
#include <brightcpp/renderer2d.hpp>
#include <memory>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

struct render_texture_info {
    vec2 pos, anchor_pos, clip_pos, clip_size, scale;
    vec4 color;
    float theta;
};

class renderer2d_backend {
  public:
    virtual ~renderer2d_backend() = default;

    virtual void render_begin() = 0;
    virtual void render_end() = 0;

    virtual void draw_texture(handle_holder<image_impl> image, const render_texture_info &info) = 0;
};

class renderer2d_factory_backend {
  public:
    virtual ~renderer2d_factory_backend() = default;

    virtual std::unique_ptr<renderer2d_backend> make(render_target rt) = 0;
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

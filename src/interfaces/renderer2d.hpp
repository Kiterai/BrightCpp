#pragma once

#include "../linear_algebra.hpp"
#include <brightcpp/image.hpp>
#include <brightcpp/render_target.hpp>
#include <memory>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

struct render_texture_info {
    vec2 pos, size, apos, clip_pos, clip_size;
    vec4 color;
    float theta;
};

class render2d_backend {
    virtual void draw_texture(handle_holder<image_impl> image, render_texture_info &info) = 0;
};

class renderer2d_factory_backend {
    virtual std::unique_ptr<render2d_backend> make(handle_holder<render_target> rt) = 0;
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

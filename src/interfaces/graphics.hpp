#pragma once

#include "../rendering_data.hpp"
#include "os_util.hpp"
#include <brightcpp/common.hpp>
#include <cstdint>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

class texture_backend {
  public:
    virtual ~texture_backend() = default;
};

class render_target_backend {
  public:
    virtual ~render_target_backend() = default;
};

class graphics_backend {
  public:
    virtual ~graphics_backend() = default;

    virtual std::unique_ptr<render_target_backend> create_render_target(window_backend &screen) = 0;
    virtual void destroy_render_target(render_target_backend &rt) noexcept = 0;
    virtual void set_current_render_target(render_target_backend &rt) = 0;

    virtual void draw(texture_backend texture, render_rect_info &info) = 0;
};

class texture_factory_backend {
  public:
    virtual ~texture_factory_backend() = default;

    virtual texture_backend make(const uint8_t *data, int w, int h) = 0;
    virtual void destroy(texture_backend image) noexcept = 0;
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

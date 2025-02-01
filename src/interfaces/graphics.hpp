#pragma once

#include "os_util.hpp"
#include <brightcpp/common.hpp>
#include <brightcpp/render_target.hpp>
#include <cstdint>

BRIGHTCPP_START

namespace internal {

class graphics_backend {
  public:
    virtual ~graphics_backend() = default;

    virtual handle_holder<render_target>::handle_value_t create_render_target(window_backend &screen) = 0;
    virtual void destroy_render_target(handle_holder<render_target> &rt) noexcept = 0;
};

} // namespace internal

BRIGHTCPP_END

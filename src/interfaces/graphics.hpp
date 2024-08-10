#pragma once

#include "os_util.hpp"
#include <brightcpp/common.hpp>
#include <brightcpp/render_target.hpp>
#include <cstdint>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

class graphics_backend {
  public:
    virtual ~graphics_backend() = default;

    virtual handle_holder<render_target>::handle_value_t create_render_target(window_backend &screen) = 0;
    virtual void destroy_render_target(handle_holder<render_target> &rt) noexcept = 0;
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

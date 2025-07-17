#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/rendertarget.hpp>
#include <brightcpp/image_decl.hpp>
#include "os_util.hpp"

BRIGHTCPP_START

namespace internal {

class rendertarget_factory_backend {
  public:
    virtual ~rendertarget_factory_backend() = default;

    virtual handle_holder<rendertarget>::handle_value_t create_render_target(window_backend &window) = 0;
    virtual handle_holder<rendertarget>::handle_value_t create_render_target(handle_holder<image_impl> image) = 0;
    virtual void destroy_render_target(handle_holder<rendertarget> &rt) noexcept = 0;
};

} // namespace internal

BRIGHTCPP_END

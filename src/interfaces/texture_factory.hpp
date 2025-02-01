#pragma once

#include "os_util.hpp"
#include <brightcpp/common.hpp>
#include <brightcpp/geometry.hpp>
#include <brightcpp/image_decl.hpp>
#include <cstdint>

BRIGHTCPP_START

namespace internal {

class texture_factory_backend {
  public:
    virtual ~texture_factory_backend() = default;

    virtual handle_holder<image_impl>::handle_value_t make(const uint8_t *data, uint32_t w, uint32_t h) = 0;
    virtual void destroy(const handle_holder<image_impl> &) noexcept = 0;

    virtual rect_size texture_size(const handle_holder<image_impl> &) const = 0;
};

} // namespace internal

BRIGHTCPP_END

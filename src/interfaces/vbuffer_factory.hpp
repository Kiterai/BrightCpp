#pragma once

#include "os_util.hpp"
#include <brightcpp/common.hpp>
#include <brightcpp/geometry.hpp>
#include <brightcpp/image_decl.hpp>
#include <cstdint>

BRIGHTCPP_START

namespace internal {

class vbuffer_impl;

class vbuffer_factory_backend {
  public:
    virtual ~vbuffer_factory_backend() = default;

    virtual handle_holder<vbuffer_impl>::handle_value_t make(const uint8_t *data, size_t bytes_num) = 0;
    virtual void destroy(const handle_holder<vbuffer_impl> &) noexcept = 0;
};

} // namespace internal

BRIGHTCPP_END

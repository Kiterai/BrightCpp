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

    virtual entity_handle_t make(size_t bytes_num) = 0;
    virtual void update_data(entity_handle_t handle, const uint8_t *data, size_t bytes_num) = 0;
    virtual void destroy(entity_handle_t) noexcept = 0;
};

} // namespace internal

BRIGHTCPP_END

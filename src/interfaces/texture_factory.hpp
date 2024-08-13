#pragma once

#include "os_util.hpp"
#include <brightcpp/common.hpp>
#include <brightcpp/image_decl.hpp>
#include <cstdint>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

class texture_factory_backend {
  public:
    virtual ~texture_factory_backend() = default;

    virtual handle_holder<image_impl>::handle_value_t make(const uint8_t *data, uint32_t w, uint32_t h) = 0;
    virtual void destroy(handle_holder<image_impl> &) noexcept = 0;

    virtual rect_size texture_size(handle_holder<image_impl> &) = 0;
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

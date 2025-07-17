#pragma once

#include "os_util.hpp"
#include <brightcpp/common.hpp>
#include <brightcpp/rendertarget.hpp>
#include <cstdint>

BRIGHTCPP_START

namespace internal {

class graphics_backend {
  public:
    virtual ~graphics_backend() = default;
};

} // namespace internal

BRIGHTCPP_END

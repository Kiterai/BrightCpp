#pragma once

#include <brightcpp/common.hpp>

BRIGHTCPP_START

namespace internal {

class audio_backend {
  public:
    virtual ~audio_backend() = default;
};

} // namespace internal

BRIGHTCPP_END

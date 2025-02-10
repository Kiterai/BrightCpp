#pragma once

#include <brightcpp/common.hpp>
#include <span>

BRIGHTCPP_START

namespace internal {

class audio_backend {
  public:
    virtual ~audio_backend() = default;

    virtual int get_sample_rate() = 0;
};

} // namespace internal

BRIGHTCPP_END

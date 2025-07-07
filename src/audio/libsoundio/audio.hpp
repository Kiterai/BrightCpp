#pragma once

#include "../../interfaces/audio.hpp"
#include <memory>

BRIGHTCPP_START

namespace internal {

namespace libsoundio {

std::unique_ptr<audio_backend> make_libsoundio_manager();

}

} // namespace internal

BRIGHTCPP_END

#include "loader.hpp"
#include <brightcpp/common.hpp>

BRIGHTCPP_START

namespace internal {

std::unique_ptr<audio_loader_backend> make_mp3_loader();

} // namespace internal

BRIGHTCPP_END

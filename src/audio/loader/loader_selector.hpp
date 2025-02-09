#include "loader.hpp"
#include <brightcpp/audio.hpp>
#include <brightcpp/common.hpp>
#include <memory>

BRIGHTCPP_START

namespace internal {

std::unique_ptr<audio_loader_backend> make_loader(std::filesystem::path path, audio_file_type type);

} // namespace internal

BRIGHTCPP_END

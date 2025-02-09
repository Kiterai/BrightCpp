#pragma once

#include <brightcpp/common.hpp>
#include <filesystem>
#include <vector>

BRIGHTCPP_START

namespace internal {

struct audio_loaded_result {
    std::vector<float> buf;
    float samplerate;
};

class audio_loader_backend {
  public:
    virtual std::vector<float> load_full_from_file(std::filesystem::path path) = 0;
    virtual audio_loaded_result load_full_from_file2(std::filesystem::path path) = 0;
};

} // namespace internal

BRIGHTCPP_END

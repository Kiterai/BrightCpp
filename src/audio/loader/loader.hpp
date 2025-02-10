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

struct audio_loaded_meta {
    float samplerate;
};

class audio_loader_backend {
  public:
    virtual audio_loaded_result load_full_from_file2(std::filesystem::path path) = 0;
    virtual audio_loaded_meta open(std::filesystem::path path) { throw std::runtime_error("not implemented"); }
    virtual std::vector<float> load_full() { throw std::runtime_error("not implemented"); }
    virtual size_t load_chunk(float *out, size_t max_sample) { throw std::runtime_error("not implemented"); }
};

} // namespace internal

BRIGHTCPP_END

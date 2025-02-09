#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#include "mp3.hpp"
#include <minimp3_ex.h>
#include <stdexcept>

BRIGHTCPP_START

namespace internal {

std::vector<float> load_mp3_full(const char *path) {
    std::vector<float> loaded_audio;

    mp3dec_t mp3d;
    mp3dec_file_info_t info;
    if (mp3dec_load(&mp3d, path, &info, NULL, NULL)) {
        throw std::runtime_error("failed to decode mp3");
    }

    for (int i = 0; i < info.samples; i += info.channels) {
        loaded_audio.push_back(info.buffer[i]);
    }

    info.buffer;

    free(info.buffer);

    return loaded_audio;
}

} // namespace internal

BRIGHTCPP_END

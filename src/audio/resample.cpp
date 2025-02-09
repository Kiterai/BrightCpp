#include "resample.hpp"
#include <samplerate.h>
#include <stdexcept>
#include <iostream>

BRIGHTCPP_START

namespace internal {

std::vector<float> instant_full_resample(const std::vector<float> &src, float src_samplerate, float dst_samplerate) {
    int err;
    auto state = src_new(SRC_SINC_MEDIUM_QUALITY, 1, &err);
    if (state == nullptr)
        throw std::runtime_error("failed to setup resampler: " + std::string(src_strerror(err)));

    size_t src_frames = src.size();
    size_t dst_frames = src_frames * dst_samplerate / src_samplerate;
    std::vector<float> dst(dst_frames);

    SRC_DATA data;
    data.data_in = src.data();
    data.input_frames = src_frames;
    data.data_out = dst.data();
    data.output_frames = dst_frames;
    data.src_ratio = dst_samplerate / src_samplerate;
    data.end_of_input = 1;

    err = src_process(state, &data);
    if (err != 0)
        throw std::runtime_error("failed to process resampling: " + std::string(src_strerror(err)));

    // std::cout << data.input_frames_used << " / " << src_frames << " consumed" << std::endl;
    // std::cout << data.output_frames_gen << " / " << dst_frames << " generated" << std::endl;

    src_delete(state);

    return dst;
}

} // namespace internal

BRIGHTCPP_END

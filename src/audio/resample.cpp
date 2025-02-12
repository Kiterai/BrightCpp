#include "resample.hpp"
#include <stdexcept>

BRIGHTCPP_START

namespace internal {

std::vector<float> instant_full_resample(const std::vector<float> &src, float src_samplerate, float dst_samplerate) {
    int err;
    auto state = src_new(SRC_SINC_MEDIUM_QUALITY, 1, &err);
    if (state == nullptr)
        throw std::runtime_error("failed to setup resampler: " + std::string(src_strerror(err)));

    size_t src_frames = src.size();
    size_t dst_frames = size_t(src_frames * dst_samplerate / src_samplerate);
    std::vector<float> dst(dst_frames);

    SRC_DATA data;
    data.data_in = src.data();
    data.input_frames = long(src_frames);
    data.data_out = dst.data();
    data.output_frames = long(dst_frames);
    data.src_ratio = dst_samplerate / src_samplerate;
    data.end_of_input = 1;

    err = src_process(state, &data);
    if (err != 0)
        throw std::runtime_error("failed to process resampling: " + std::string(src_strerror(err)));

    src_delete(state);

    return dst;
}

streaming_resampler::streaming_resampler(float in_samplerate, float out_samplerate) : rate{out_samplerate / in_samplerate} {
    int err;
    state = src_new(SRC_SINC_MEDIUM_QUALITY, 1, &err);
    if (state == nullptr)
        throw std::runtime_error("failed to setup resampler: " + std::string(src_strerror(err)));
}

streaming_resampler::~streaming_resampler() {
    src_delete(state);
}

size_t streaming_resampler::resample(float *out, const float *in, size_t in_frames, size_t max_out_frames) {
    for (size_t i = 0; i < in_frames; i++)
        buffer.push_back(in[i]);

    SRC_DATA data;
    data.data_in = buffer.data();
    data.input_frames = long(buffer.size());
    data.data_out = out;
    data.output_frames = long(max_out_frames);
    data.src_ratio = rate;
    data.end_of_input = 0;

    auto err = src_process(state, &data);
    if (err != 0)
        throw std::runtime_error("failed to process resampling: " + std::string(src_strerror(err)));

    for (size_t i = data.input_frames_used; i < in_frames; i++)
        buffer[i - data.input_frames_used] = buffer[i];
    buffer.resize(in_frames - data.input_frames_used);

    return data.output_frames_gen;
}

} // namespace internal

BRIGHTCPP_END

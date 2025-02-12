#include <brightcpp/common.hpp>
#include <samplerate.h>
#include <vector>

BRIGHTCPP_START

namespace internal {

std::vector<float> instant_full_resample(const std::vector<float> &src, float src_samplerate, float dst_samplerate);

class streaming_resampler {
    SRC_STATE *state;
    float rate;
    std::vector<float> buffer;

  public:
    streaming_resampler(float in_samplerate, float out_samplerate);
    ~streaming_resampler();

    size_t resample(float *out, const float *in, size_t in_frames, size_t max_out_frames);
};

} // namespace internal

BRIGHTCPP_END

#include <brightcpp/common.hpp>
#include <vector>

BRIGHTCPP_START

namespace internal {

std::vector<float> instant_full_resample(const std::vector<float> &src, int src_samplerate, int dst_samplerate);

} // namespace internal

BRIGHTCPP_END

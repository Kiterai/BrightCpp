#include <brightcpp/common.hpp>
#include <filesystem>
#include <vector>

BRIGHTCPP_START

namespace internal {

class audio_loader_backend {
  public:
    virtual std::vector<float> load_full_from_file(std::filesystem::path path) = 0;
};

} // namespace internal

BRIGHTCPP_END

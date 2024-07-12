#include <brightcpp/image.hpp>
#include <brightcpp/internal/vulkan.hpp>
#include <stb_image.h>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

class image_impl {

  public:
    image_impl(const char *path) {
        int w, h, ch;
        const auto data = stbi_load(path, &w, &h, &ch, STBI_rgb_alpha);

        stbi_image_free(data);
    }
};

} // namespace internal

image_clip::image_clip(std::reference_wrapper<internal::image_impl> p)
    : p_impl(p) {
}
image_clip::~image_clip() = default;

image::image(const char *path)
    : p_impl{std::make_unique<internal::image_impl>(path)} {}
image::~image() = default;

} // namespace BRIGHTCPP_NAMESPACE

#include <stb_image.h>

#include <brightcpp/image.hpp>
#include "vulkan/texture.hpp"
#include "vulkan/vulkan.hpp"

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

class image_impl {
  texture_resource tex;

 public:
  image_impl(const char *path) {
    int w, h, ch;
    const auto data = stbi_load(path, &w, &h, &ch, STBI_rgb_alpha);

    // TODO
    // tex = factory->create_texture(data, w, h);

    stbi_image_free(data);
  }
};

}  // namespace internal

image_clip::image_clip(const std::weak_ptr<internal::image_impl> &p, int cx,
                       int cy, int cw, int ch)
    : p_impl(p), cx{cx}, cy{cy}, cw{cw}, ch{ch} {}
image_clip::~image_clip() = default;

image_clip image_clip::clip(int x, int y, int w, int h) {
  return image_clip{
      p_impl, cx + x, cy + y, std::min(w, cw - x), std::min(h, ch - y),
  };
}

image::image(const char *path)
    : p_impl{std::make_unique<internal::image_impl>(path)} {}
image::~image() = default;

image_clip image::clip(int x, int y, int w, int h) {
  return image_clip{
      p_impl, x, y, w, h,
  };
}

}  // namespace BRIGHTCPP_NAMESPACE

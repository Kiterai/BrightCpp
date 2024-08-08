#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "interfaces/graphics.hpp"
#include "system.hpp"
#include <brightcpp/image.hpp>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

using g_tex_factory = global_module<texture_factory_backend>;

class image_impl {
    std::unique_ptr<texture_backend> tex;

  public:
    image_impl(const char *path) {
        int w, h, ch;
        const auto data = stbi_load(path, &w, &h, &ch, STBI_rgb_alpha);
        if(data == nullptr)
            throw std::runtime_error(std::string("failed to load image file: ") + path);

        tex = g_tex_factory::get().make(data, w, h);

        stbi_image_free(data);
    }
};

} // namespace internal

image_clip::image_clip(const std::weak_ptr<internal::image_impl> &p, int cx,
                       int cy, int cw, int ch)
    : p_impl(p), cx{cx}, cy{cy}, cw{cw}, ch{ch} {}
image_clip::~image_clip() = default;

image_clip image_clip::clip(int x, int y, int w, int h) {
    return image_clip{
        p_impl,
        cx + x,
        cy + y,
        std::min(w, cw - x),
        std::min(h, ch - y),
    };
}

image::image(const char *path)
    : p_impl{std::make_unique<internal::image_impl>(path)} {}
image::~image() = default;

image_clip image::clip(int x, int y, int w, int h) {
    return image_clip{
        p_impl,
        x,
        y,
        w,
        h,
    };
}

} // namespace BRIGHTCPP_NAMESPACE

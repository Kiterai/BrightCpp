#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "global_module.hpp"
#include "interfaces/graphics.hpp"
#include <brightcpp/image.hpp>

namespace BRIGHTCPP_NAMESPACE {

using g_tex_factory = internal::global_module<internal::texture_factory_backend>;

static auto load_image_from_file(std::string path) {
    int w, h, ch;
    const auto data = stbi_load(path.c_str(), &w, &h, &ch, STBI_rgb_alpha);
    if (data == nullptr)
        throw std::runtime_error(std::string("failed to load image file: ") + path);

    auto handle = g_tex_factory::get().make(data, w, h);

    stbi_image_free(data);

    return handle;
}

image_clip::image_clip(handle_holder<image_impl> _handle, int cx, int cy, int cw, int ch)
    : handle_holder(_handle), cx{cx}, cy{cy}, cw{cw}, ch{ch} {}
image_clip::~image_clip() = default;

image_clip image_clip::clip(int x, int y, int w, int h) {
    return image_clip{
        *this,
        cx + x,
        cy + y,
        std::min(w, cw - x),
        std::min(h, ch - y),
    };
}

image::image(const char *path) : handle_holder{load_image_from_file(path)} {}
image::~image() {
    g_tex_factory::get().destroy(*this);
};

image_clip image::clip(int x, int y, int w, int h) {
    return image_clip{
        *this,
        x,
        y,
        w,
        h,
    };
}

} // namespace BRIGHTCPP_NAMESPACE

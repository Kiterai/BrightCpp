#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../global_module.hpp"
#include "../interfaces/texture_factory.hpp"
#include "../interfaces/rendertarget_factory.hpp"
#include <brightcpp/image.hpp>

BRIGHTCPP_START

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

static auto make_empty_image(int w, int h) {
    std::vector<uint8_t> data(w * h * 4, 0);
    return g_tex_factory::get().make(data.data(), w, h);
}

image_clip::image_clip(handle_holder<image_impl> _handle, int cx, int cy, int cw, int ch)
    : handle_holder(_handle), cx{cx}, cy{cy}, cw{cw}, ch{ch} {}
image_clip::~image_clip() = default;

rect image_clip::clipping_area() const {
    return rect(cx, cy, cw, ch);
}
image_clip image_clip::clip(int x, int y, int w, int h) const {
    return image_clip{
        *this,
        cx + x,
        cy + y,
        std::min(w, cw - x),
        std::min(h, ch - y),
    };
}

image::image(const char *path) : handle_holder{load_image_from_file(path)} {}
image::image(int w, int h) : handle_holder{make_empty_image(w, h)} {}
image::~image() {
    g_tex_factory::get().destroy(*this);
};

rect_size image::size() const {
    return g_tex_factory::get().texture_size(*this);
}
image_clip image::clip() const {
    const auto _size = size();
    return image_clip{*this, 0, 0, _size.width(), _size.height()};
}
image_clip image::clip(int x, int y, int w, int h) const {
    return image_clip{*this, x, y, w, h};
}

using g_rt_factory = internal::global_module<internal::rendertarget_factory_backend>;

rendertarget image::get_render_target() const {
    return rendertarget{ g_rt_factory::get().create_render_target(*this) };
}

BRIGHTCPP_END

#pragma once

#include "../entity_holder.hpp"
#include "../global_module.hpp"
#include "../interfaces/renderer2d.hpp"
#include "vbuffer.hpp"
#include <brightcpp/common.hpp>
#include <brightcpp/color.hpp>

BRIGHTCPP_START

namespace internal {

class renderer2d_middleware {
    std::unique_ptr<renderer2d_backend> p_renderer;
    vbuffer rect_buffer;
    image empty_texture;
    rendertarget rt;

  public:
    renderer2d_middleware(rendertarget rt);
    ~renderer2d_middleware();

    void draw_texture(const image &image, int x, int y, float rotation, float scale_x, float scale_y, pivot image_pivot);
    void draw_texture(const image_clip &image, int x, int y, float rotation, float scale_x, float scale_y, pivot image_pivot);

    void draw_rect(color fill_color, int x, int y, float rotation, float width, float height, pivot pivot);
    void draw_circle();

    void flush();
};

class renderer2d_middleware_manager : public entity_holder<renderer2d_middleware> {};

} // namespace internal

BRIGHTCPP_END

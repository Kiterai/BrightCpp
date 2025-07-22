#pragma once

#include "../global_module.hpp"
#include "../interfaces/renderer2d.hpp"
#include "../entity_holder.hpp"
#include <brightcpp/common.hpp>

BRIGHTCPP_START

namespace internal {

class renderer2d_middleware {
    std::unique_ptr<renderer2d_backend> p_renderer;

  public:
    renderer2d_middleware(rendertarget rt);
    ~renderer2d_middleware();

    void draw_texture(image &image, int x, int y, float rotation, float scale_x, float scale_y, pivot image_pivot);
    void draw_texture(image_clip &image, int x, int y, float rotation, float scale_x, float scale_y, pivot image_pivot);

    void draw_rect();
    void draw_circle();

    void flush();
};

class renderer2d_middleware_manager : public entity_holder<renderer2d_middleware> {};

} // namespace internal

BRIGHTCPP_END

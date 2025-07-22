#include "../interfaces/renderer2d.hpp"
#include "../global_module.hpp"
#include "../interfaces/graphics.hpp"
#include "renderer2d_middleware.hpp"
#include <brightcpp/renderer2d.hpp>

BRIGHTCPP_START

using g_renderer_middleware_manager_t = internal::global_module<internal::renderer2d_middleware_manager>;

renderer2d::renderer2d(rendertarget rt) : handle_holder{g_renderer_middleware_manager_t::get().make(rt)} {}
renderer2d::~renderer2d() { g_renderer_middleware_manager_t::get().destroy(handle()); };

renderer2d &renderer2d::draw(image &image, int x, int y) {
    return draw(image, x, y, 0.0f, 1.0f, 1.0f, pivot_pos::left_top);
}
renderer2d &renderer2d::draw(image_clip &clip, int x, int y) {
    return draw(clip, x, y, 0.0f, 1.0f, 1.0f, pivot_pos::left_top);
}
renderer2d &renderer2d::draw(image &image, int x, int y, float rotation) {
    return draw(image, x, y, rotation, 1.0f, 1.0f, pivot_pos::left_top);
}
renderer2d &renderer2d::draw(image_clip &clip, int x, int y, float rotation) {
    return draw(clip, x, y, rotation, 1.0f, 1.0f, pivot_pos::left_top);
}
renderer2d &renderer2d::draw(image &image, int x, int y, float scale_x, float scale_y) {
    return draw(image, x, y, 0.0f, scale_x, scale_y, pivot_pos::left_top);
}
renderer2d &renderer2d::draw(image_clip &clip, int x, int y, float scale_x, float scale_y) {
    return draw(clip, x, y, 0.0f, scale_x, scale_y, pivot_pos::left_top);
}
renderer2d &renderer2d::draw(image &image, int x, int y, float rotation, float scale_x, float scale_y) {
    return draw(image, x, y, rotation, scale_x, scale_y, pivot_pos::left_top);
}
renderer2d &renderer2d::draw(image_clip &clip, int x, int y, float rotation, float scale_x, float scale_y) {
    return draw(clip, x, y, rotation, scale_x, scale_y, pivot_pos::left_top);
}

renderer2d &renderer2d::draw(image &image, int x, int y, pivot image_pivot) {
    return draw(image, x, y, 0.0f, 1.0f, 1.0f, image_pivot);
}
renderer2d &renderer2d::draw(image_clip &clip, int x, int y, pivot clip_pivot) {
    return draw(clip, x, y, 0.0f, 1.0f, 1.0f, clip_pivot);
}
renderer2d &renderer2d::draw(image &image, int x, int y, float rotation, pivot image_pivot) {
    return draw(image, x, y, rotation, 1.0f, 1.0f, image_pivot);
}
renderer2d &renderer2d::draw(image_clip &clip, int x, int y, float rotation, pivot clip_pivot) {
    return draw(clip, x, y, rotation, 1.0f, 1.0f, clip_pivot);
}
renderer2d &renderer2d::draw(image &image, int x, int y, float scale_x, float scale_y, pivot image_pivot) {
    return draw(image, x, y, 0.0f, scale_x, scale_y, image_pivot);
}
renderer2d &renderer2d::draw(image_clip &clip, int x, int y, float scale_x, float scale_y, pivot clip_pivot) {
    return draw(clip, x, y, 0.0f, scale_x, scale_y, clip_pivot);
}

renderer2d &renderer2d::draw(image &image, int x, int y, float rotation, float scale_x, float scale_y,
                             pivot image_pivot) {
    g_renderer_middleware_manager_t::get().get(handle()).draw_texture(image, x, y, rotation, scale_x, scale_y,
                                                                      image_pivot);
    return *this;
}
renderer2d &renderer2d::draw(image_clip &clip, int x, int y, float rotation, float scale_x, float scale_y,
                             pivot clip_pivot) {
    g_renderer_middleware_manager_t::get().get(handle()).draw_texture(clip, x, y, rotation, scale_x, scale_y,
                                                                      clip_pivot);
    return *this;
}

renderer2d &renderer2d::flush() {
    g_renderer_middleware_manager_t::get().get(handle()).flush();
    return *this;
}

BRIGHTCPP_END

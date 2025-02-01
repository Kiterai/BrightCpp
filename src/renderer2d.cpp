#include "interfaces/renderer2d.hpp"
#include "global_module.hpp"
#include "interfaces/graphics.hpp"
#include <brightcpp/renderer2d.hpp>

BRIGHTCPP_START

using g_rendererfactory_t = internal::global_module<internal::renderer2d_factory_backend>;

renderer2d::renderer2d(render_target rt)
    : p_renderer(g_rendererfactory_t::get().make(rt)) {
    p_renderer->render_begin();
}
renderer2d::~renderer2d() = default;

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

renderer2d &renderer2d::draw(image &image, int x, int y, float rotation, float scale_x, float scale_y, pivot image_pivot) {
    const auto size = image.size();

    p_renderer->draw_texture(
        image,
        {
            .pos{float(x), float(y)},
            .anchor_pos{image_pivot.x.value, image_pivot.y.value},
            .clip_pos{0.0f, 0.0f},
            .clip_size{float(size.width()), float(size.height())},
            .scale{scale_x, scale_y},
            .color{1.0f, 0.0f, 0.0f, 1.0f},
            .theta{rotation},
        });
    return *this;
}
renderer2d &renderer2d::draw(image_clip &clip, int x, int y, float rotation, float scale_x, float scale_y, pivot clip_pivot) {
    const auto clip_rect = clip.clipping_area();

    p_renderer->draw_texture(
        clip,
        {
            .pos{float(x), float(y)},
            .anchor_pos{clip_pivot.x.value, clip_pivot.y.value},
            .clip_pos{float(clip_rect.left()), float(clip_rect.top())},
            .clip_size{float(clip_rect.width()), float(clip_rect.height())},
            .scale{scale_x, scale_y},
            .color{1.0f, 0.0f, 0.0f, 1.0f},
            .theta{rotation},
        });
    return *this;
}

renderer2d &renderer2d::flush() {
    p_renderer->render_end();
    p_renderer->render_begin();

    return *this;
}

BRIGHTCPP_END

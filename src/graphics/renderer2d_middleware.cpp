#pragma once

#include "renderer2d_middleware.hpp"

BRIGHTCPP_START

namespace internal {

using g_rendererfactory_t = internal::global_module<internal::renderer2d_factory_backend>;

renderer2d_middleware::renderer2d_middleware(rendertarget rt) {
    p_renderer = g_rendererfactory_t::get().make(rt);
    p_renderer->render_begin();
}
renderer2d_middleware::~renderer2d_middleware() { p_renderer->render_end(); }

void renderer2d_middleware::draw_texture(image &image, int x, int y, float rotation, float scale_x, float scale_y,
                                         pivot image_pivot) {
    const auto size = image.size();

    p_renderer->draw_texture(image, {
                                        .pos{float(x), float(y)},
                                        .anchor_pos{image_pivot.x.value, image_pivot.y.value},
                                        .clip_pos{0.0f, 0.0f},
                                        .clip_size{float(size.width()), float(size.height())},
                                        .scale{scale_x, scale_y},
                                        .color{1.0f, 0.0f, 0.0f, 1.0f},
                                        .theta{rotation},
                                    });
}

void renderer2d_middleware::draw_texture(image_clip &clip, int x, int y, float rotation, float scale_x, float scale_y,
                                         pivot clip_pivot) {
    const auto clip_rect = clip.clipping_area();

    p_renderer->draw_texture(clip, {
                                       .pos{float(x), float(y)},
                                       .anchor_pos{clip_pivot.x.value, clip_pivot.y.value},
                                       .clip_pos{float(clip_rect.left()), float(clip_rect.top())},
                                       .clip_size{float(clip_rect.width()), float(clip_rect.height())},
                                       .scale{scale_x, scale_y},
                                       .color{1.0f, 0.0f, 0.0f, 1.0f},
                                       .theta{rotation},
                                   });
};

void renderer2d_middleware::draw_rect() {
    // TODO
}
void renderer2d_middleware::draw_circle() {
    // TODO
}

void renderer2d_middleware::flush() {
    p_renderer->render_end();
    p_renderer->render_begin();
}

} // namespace internal

BRIGHTCPP_END

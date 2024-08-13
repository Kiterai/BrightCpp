#include "interfaces/renderer2d.hpp"
#include "global_module.hpp"
#include "interfaces/graphics.hpp"
#include <brightcpp/renderer2d.hpp>

namespace BRIGHTCPP_NAMESPACE {

using g_rendererfactory_t = internal::global_module<internal::renderer2d_factory_backend>;

renderer2d::renderer2d(render_target rt)
    : p_renderer(g_rendererfactory_t::get().make(rt)) {
    p_renderer->render_begin();
}
renderer2d::~renderer2d() = default;

renderer2d &renderer2d::draw(image &image, int x, int y) {
    const auto size = image.size();

    p_renderer->draw_texture(
        image,
        {
            .pos{float(x), float(y)},
            .anchor_pos{0, 0},
            .clip_pos{0.0f, 0.0f},
            .clip_size{float(size.width()), float(size.height())},
            .color{1.0f, 0.0f, 0.0f, 1.0f},
        });
    return *this;
}
renderer2d &renderer2d::draw(image_clip &clip, int x, int y) {
    const auto clip_rect = clip.clipping_area();

    p_renderer->draw_texture(
        clip,
        {
            .pos{float(x), float(y)},
            .anchor_pos{0, 0},
            .clip_pos{float(clip_rect.left()), float(clip_rect.top())},
            .clip_size{float(clip_rect.width()), float(clip_rect.height())},
            .color{1.0f, 0.0f, 0.0f, 1.0f},
        });
    return *this;
}

renderer2d &renderer2d::flush() {
    p_renderer->render_end();
    p_renderer->render_begin();

    return *this;
}

} // namespace BRIGHTCPP_NAMESPACE

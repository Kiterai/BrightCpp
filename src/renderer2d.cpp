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

renderer2d &renderer2d::draw(image &image, int x, int y) {
    p_renderer->draw_texture(
        image,
        {
            .pos{float(x), float(y)},
            .size{100, 100}, // TODO
            .apos{0, 0},
            .color{1.0f, 0.0f, 0.0f, 1.0f},
        });
    return *this;
}
renderer2d &renderer2d::draw(image_clip &clip, int x, int y) {
    p_renderer->draw_texture(
        clip,
        {
            .pos{float(x), float(y)},
            .size{100, 100}, // TODO
            .apos{0, 0},
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

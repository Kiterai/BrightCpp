#include "interfaces/renderer2d.hpp"
#include "global_module.hpp"
#include "interfaces/graphics.hpp"
#include <brightcpp/renderer2d.hpp>

namespace BRIGHTCPP_NAMESPACE {

using g_rendererfactory_t = internal::global_module<internal::renderer2d_factory_backend>;

renderer2d::renderer2d(render_target rt)
    : handle_holder(g_rendererfactory_t::get().make(rt)) {
    g_rendererfactory_t::get().get(*this).render_begin();
}

renderer2d &renderer2d::draw(image image, int x, int y) {
    g_rendererfactory_t::get().get(*this).draw_texture(
        image,
        {
            .pos{float(x), float(y)},
            .size{100, 100}, // TODO
            .apos{0, 0},
            .color{1.0f, 0.0f, 0.0f, 1.0f},
        });
    return *this;
}
renderer2d &renderer2d::draw(image_clip clip, int x, int y) {
    g_rendererfactory_t::get().get(*this).draw_texture(
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
    g_rendererfactory_t::get().get(*this).render_end();
    g_rendererfactory_t::get().get(*this).render_begin();

    return *this;
}

} // namespace BRIGHTCPP_NAMESPACE

#include "global_module.hpp"
#include "interfaces/graphics.hpp"
#include "interfaces/renderer2d.hpp"
#include <brightcpp/renderer.hpp>

namespace BRIGHTCPP_NAMESPACE {

renderer2d::renderer2d(render_target rt)
    : handle_holder(internal::global_module<internal::renderer2d_factory_backend>::get().make(rt)) {
}

renderer2d &renderer2d::draw(image image, int x, int y) {
}
renderer2d &renderer2d::draw(image_clip clip, int x, int y) {
}

} // namespace BRIGHTCPP_NAMESPACE

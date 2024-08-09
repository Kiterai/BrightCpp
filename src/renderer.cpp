#include <brightcpp/renderer.hpp>

namespace BRIGHTCPP_NAMESPACE {

render_target &renderer::target() const {
}
render_target &renderer::target(render_target &new_target, render_mode mode = render_mode::graphics2d) {
}

renderer &renderer::draw(image image) {
}
renderer &renderer::draw(image_clip clip) {
}

} // namespace BRIGHTCPP_NAMESPACE

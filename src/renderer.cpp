#include <brightcpp/renderer.hpp>

namespace BRIGHTCPP_NAMESPACE {

render_target &renderer::target() const {
}
render_target &renderer::target(render_target &new_target, render_mode mode = render_mode::graphics2d) {
}

renderer &renderer::draw(image image, int x, int y) {
}
renderer &renderer::draw(image_clip clip, int x, int y) {
}

} // namespace BRIGHTCPP_NAMESPACE

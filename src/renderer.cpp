#include "global_module.hpp"
#include "interfaces/graphics.hpp"
#include <brightcpp/renderer.hpp>

namespace BRIGHTCPP_NAMESPACE {

renderer::renderer(render_target rt) : handle_holder(0) // TODO
{
}

renderer &renderer::draw(image image, int x, int y) {
}
renderer &renderer::draw(image_clip clip, int x, int y) {
}

} // namespace BRIGHTCPP_NAMESPACE

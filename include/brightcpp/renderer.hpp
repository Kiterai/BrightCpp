#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/image.hpp>
#include <brightcpp/render_target.hpp>
#include <functional>

namespace BRIGHTCPP_NAMESPACE {

class renderer : handle_holder<renderer> {
  public:
    renderer(render_target render_to);

    renderer &draw(image image, int x, int y);
    renderer &draw(image_clip clip, int x, int y);
};

} // namespace BRIGHTCPP_NAMESPACE

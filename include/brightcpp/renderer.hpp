#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/image.hpp>
#include <brightcpp/render_target.hpp>

namespace BRIGHTCPP_NAMESPACE {

class renderer2d : public handle_holder<renderer2d> {
  public:
    renderer2d(render_target render_to);

    renderer2d &draw(image image, int x, int y);
    renderer2d &draw(image_clip clip, int x, int y);
};

} // namespace BRIGHTCPP_NAMESPACE

#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/image.hpp>
#include <brightcpp/render_target.hpp>
#include <functional>

namespace BRIGHTCPP_NAMESPACE {

enum class render_mode {
    graphics2d,
    graphics3d,
    graphics2d_z,
};

class renderer {
  public:
    render_target &target() const;
    render_target &target(render_target &new_target, render_mode mode = render_mode::graphics2d);

    template <class T>
    render_target &target(T &new_target_source, render_mode mode = render_mode::graphics2d) {
        return target(new_target_source.get_render_target(), mode);
    }

    renderer &draw(image image, int x, int y);
    renderer &draw(image_clip clip, int x, int y);
};

extern renderer gout;

} // namespace BRIGHTCPP_NAMESPACE

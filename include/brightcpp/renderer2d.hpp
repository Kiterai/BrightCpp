#pragma once

#include "color.hpp"
#include <brightcpp/common.hpp>
#include <brightcpp/handle_holder.hpp>
#include <brightcpp/image.hpp>
#include <brightcpp/rendertarget.hpp>
#include <memory>

BRIGHTCPP_START

namespace internal {
class renderer2d_backend;
}

class renderer2d : handle_holder<renderer2d> {
  public:
    renderer2d(rendertarget render_to);
    ~renderer2d();

    template <class T> renderer2d(T &renderable) : renderer2d(renderable.get_render_target()) {}

    renderer2d &draw(image &image, int x, int y);
    renderer2d &draw(image_clip &clip, int x, int y);
    renderer2d &draw(image &image, int x, int y, float rotation);
    renderer2d &draw(image_clip &clip, int x, int y, float rotation);
    renderer2d &draw(image &image, int x, int y, float scale_x, float scale_y);
    renderer2d &draw(image_clip &clip, int x, int y, float scale_x, float scale_y);
    renderer2d &draw(image &image, int x, int y, float rotation, float scale_x, float scale_y);
    renderer2d &draw(image_clip &clip, int x, int y, float rotation, float scale_x, float scale_y);

    renderer2d &draw(image &image, int x, int y, pivot image_pivot);
    renderer2d &draw(image_clip &clip, int x, int y, pivot clip_pivot);
    renderer2d &draw(image &image, int x, int y, float rotation, pivot image_pivot);
    renderer2d &draw(image_clip &clip, int x, int y, float rotation, pivot clip_pivot);
    renderer2d &draw(image &image, int x, int y, float scale_x, float scale_y, pivot clip_pivot);
    renderer2d &draw(image_clip &clip, int x, int y, float scale_x, float scale_y, pivot clip_pivot);
    renderer2d &draw(image &image, int x, int y, float rotation, float scale_x, float scale_y, pivot clip_pivot);
    renderer2d &draw(image_clip &clip, int x, int y, float rotation, float scale_x, float scale_y, pivot clip_pivot);

    renderer2d &draw_filled_rect(color fill_color, int x, int y, int width, int height);

    renderer2d &flush();
};

BRIGHTCPP_END

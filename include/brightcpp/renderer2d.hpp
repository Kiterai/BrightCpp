#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/image.hpp>
#include <brightcpp/render_target.hpp>
#include <memory>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {
class renderer2d_backend;
}

class renderer2d {
    std::unique_ptr<internal::renderer2d_backend> p_renderer;

  public:
    renderer2d(render_target render_to);
    ~renderer2d();

    template <class T>
    renderer2d(T &renderable) : renderer2d(renderable.get_render_target()) {}

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

    renderer2d &flush();
};

} // namespace BRIGHTCPP_NAMESPACE

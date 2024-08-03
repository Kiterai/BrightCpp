#include <brightcpp/common.hpp>
#include <cstdint>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

class texture_backend {
  public:
};

class screen_backend {
  public:
};

class render_target_backend {
  public:
};

class graphics_backend {
  public:
    virtual render_target_backend create_render_target(screen_backend screen) = 0;
    virtual void destroy_render_target(render_target_backend rt) noexcept = 0;
    virtual void set_current_render_target(render_target_backend rt) = 0;

    virtual void draw(texture_backend texture) = 0;
};

class texture_factory_backend {
  public:
    virtual texture_backend make(const uint8_t *data, int w, int h) = 0;
    virtual void destroy(texture_backend image) noexcept = 0;
};

}  // namespace internal

}  // namespace BRIGHTCPP_NAMESPACE

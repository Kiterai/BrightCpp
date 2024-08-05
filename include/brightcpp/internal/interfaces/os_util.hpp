#include <brightcpp/common.hpp>
#include <brightcpp/window.hpp>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

class window_backend {
  public:
    virtual ~window_backend() = default;

    virtual vk::UniqueSurfaceKHR get_vulkan_surface(vk::Instance instance) {
        throw std::exception("not implemented get_vulkan_surface()");
    }
    virtual bool is_close_requested() = 0;
};

class os_util_backend {
  public:
    virtual ~os_util_backend() = default;

    virtual std::unique_ptr<window_backend> create_window(window::settings settings) = 0;
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
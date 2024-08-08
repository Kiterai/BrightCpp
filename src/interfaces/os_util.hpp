#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/window.hpp>
#include <optional>
#include <vector>
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

    virtual void set_size(int w, int h) = 0;
    virtual std::tuple<int, int> get_size() const = 0;

    virtual void set_resizable(bool is_resizable) = 0;
    virtual bool is_resizable() const = 0;

    virtual void set_fullscreen(bool is_fullscreen) = 0;
    virtual bool is_fullscreen() const = 0;

    virtual void set_title(const std::string &title) = 0;
    virtual std::string get_title() const = 0;
};

class os_util_backend {
  public:
    virtual ~os_util_backend() = default;

    virtual std::unique_ptr<window_backend> create_window(const window::settings &settings) = 0;

    virtual std::vector<const char *> get_vulkan_required_instance_extensions() {
        throw std::exception("not implemented get_vulkan_required_instance_extensions()");
    }

    virtual void poll_events() = 0;
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
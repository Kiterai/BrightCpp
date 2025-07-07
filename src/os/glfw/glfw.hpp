#pragma once

#define GLFW_INCLUDE_VULKAN
#include "../../interfaces/os_util.hpp"
#include <brightcpp/common.hpp>
#include <memory>
#include <stdexcept>

#define BRIGHTCPP_OSUTIL_GLFW_START \
    BRIGHTCPP_START                 \
    namespace internal {            \
    namespace glfw {

#define BRIGHTCPP_OSUTIL_GLFW_END \
    }                             \
    }                             \
    BRIGHTCPP_END

BRIGHTCPP_OSUTIL_GLFW_START

class os_util_backend_glfw : public os_util_backend {
  public:
    os_util_backend_glfw();
    ~os_util_backend_glfw();
    std::unique_ptr<window_backend> create_window(const window::settings &settings) override;
    std::vector<const char *> get_vulkan_required_instance_extensions() override;
    void poll_events() override;
};

BRIGHTCPP_OSUTIL_GLFW_END

#pragma once

#define GLFW_INCLUDE_VULKAN
#include "../interfaces/os_util.hpp"
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

std::unique_ptr<os_util_backend> make_glfw_manager();

BRIGHTCPP_OSUTIL_GLFW_END

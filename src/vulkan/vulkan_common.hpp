#pragma once

#include <brightcpp/common.hpp>

#define BRIGHTCPP_GRAPHICS_VULKAN_START \
    BRIGHTCPP_START                     \
    namespace internal {                \
    namespace vulkan {

#define BRIGHTCPP_GRAPHICS_VULKAN_END \
    }                                 \
    }                                 \
    BRIGHTCPP_END

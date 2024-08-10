#pragma once

#include <brightcpp/common.hpp>

#define BRIGHTCPP_GRAPHICS_VULKAN_START \
    namespace BRIGHTCPP_NAMESPACE {     \
    namespace internal {                \
    namespace vulkan {

#define BRIGHTCPP_GRAPHICS_VULKAN_END \
    }                                 \
    }                                 \
    }

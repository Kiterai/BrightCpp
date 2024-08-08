#pragma once

#include "../interfaces/graphics.hpp"
#include <brightcpp/common.hpp>
#include <memory>

#define BRIGHTCPP_GRAPHICS_VULKAN_START \
    namespace BRIGHTCPP_NAMESPACE {     \
    namespace internal {                \
    namespace vulkan {

#define BRIGHTCPP_GRAPHICS_VULKAN_END \
    }                                 \
    }                                 \
    }

BRIGHTCPP_GRAPHICS_VULKAN_START

std::unique_ptr<graphics_backend> make_graphics_vulkan();

BRIGHTCPP_GRAPHICS_VULKAN_END

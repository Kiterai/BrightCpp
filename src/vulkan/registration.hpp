#pragma once

#include "vulkan_common.hpp"
#include <memory>

BRIGHTCPP_GRAPHICS_VULKAN_START

struct object_container {
    virtual ~object_container() = default;
};

std::unique_ptr<object_container> register_objects();

BRIGHTCPP_GRAPHICS_VULKAN_END

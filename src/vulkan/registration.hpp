#pragma once

#include "vulkan_common.hpp"
#include "../object_container.hpp"
#include <memory>

BRIGHTCPP_GRAPHICS_VULKAN_START

std::unique_ptr<object_container> register_objects();

BRIGHTCPP_GRAPHICS_VULKAN_END

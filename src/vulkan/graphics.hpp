#pragma once

#include "../interfaces/graphics.hpp"
#include "vulkan_common.hpp"
#include <brightcpp/common.hpp>
#include <memory>

BRIGHTCPP_GRAPHICS_VULKAN_START

std::unique_ptr<graphics_backend> make_graphics_vulkan(const std::shared_ptr<os_util_backend> &os_util);

BRIGHTCPP_GRAPHICS_VULKAN_END

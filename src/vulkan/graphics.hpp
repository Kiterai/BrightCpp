#pragma once

#include "../interfaces/graphics.hpp"
#include "vulkan_common.hpp"
#include <brightcpp/common.hpp>
#include <memory>

BRIGHTCPP_GRAPHICS_VULKAN_START

std::unique_ptr<graphics_backend> make_graphics_vulkan();

BRIGHTCPP_GRAPHICS_VULKAN_END

#pragma once

#include <brightcpp/common.hpp>
#include "../interfaces/graphics.hpp"
#include <memory>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

std::unique_ptr<graphics_backend> make_graphics_vulkan();

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

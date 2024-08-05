#pragma once

#define GLFW_INCLUDE_VULKAN
#include "../interfaces/os_util.hpp"
#include <brightcpp/common.hpp>
#include <memory>
#include <stdexcept>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

std::unique_ptr<os_util_backend> make_glfw_manager();

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

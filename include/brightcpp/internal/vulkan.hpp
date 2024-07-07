#pragma once

#include <brightcpp/internal/system.hpp>
#include <memory>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

std::unique_ptr<system_module> make_vulkan_manager();

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

#include <brightcpp/internal/glfw.hpp>
#include <brightcpp/internal/system.hpp>
#include <brightcpp/internal/vulkan.hpp>
#include <iostream>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

static size_t initializer_count = 0;

system_initializer::system_initializer() {
    if (initializer_count == 0) {
        setup_glfw();
        setup_vulkan_manager();
#ifdef _DEBUG
        std::cout << "successfully initialized BrightCpp." << std::endl;
#endif
    }
    initializer_count++;
}

system_initializer::~system_initializer() {
    initializer_count--;
    if (initializer_count == 0) {
#ifdef _DEBUG
        std::cout << "shutdown BrightCpp..." << std::endl;
#endif
        shutdown_vulkan_manager();
        shutdown_glfw();
    }
}

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

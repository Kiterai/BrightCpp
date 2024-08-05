#include <brightcpp/internal/system.hpp>
#include <brightcpp/internal/vulkan/vulkan.hpp>
#include <iostream>
#include <optional>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

static size_t initializer_count = 0;
static std::optional<system_container> global_container_obj;

system_initializer::system_initializer() {
    if (initializer_count == 0) {
        // COMMENTED OUT FOR ARCHITECTURE CHANGE

        // setup_glfw();
        // setup_vulkan_manager();
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
        // COMMENTED OUT FOR ARCHITECTURE CHANGE

        // shutdown_vulkan_manager();
        // shutdown_glfw();
    }
}

system_container& system_initializer::global_container() {
    return global_container_obj.value();
}

}  // namespace internal

}  // namespace BRIGHTCPP_NAMESPACE

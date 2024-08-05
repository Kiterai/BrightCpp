#include <brightcpp/internal/glfw.hpp>
#include <iostream>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

void setup_glfw() {
    BRIGHTCPP_GLFW_CHK_ERR(glfwInit());
#ifdef _DEBUG
    std::cout << "GLFW initialized." << std::endl;
#endif
}

void shutdown_glfw() {
#ifdef _DEBUG
    std::cout << "GLFW shutdown..." << std::endl;
#endif
    glfwTerminate();
}

vk::UniqueSurfaceKHR window_backend_glfw::get_vulkan_surface(vk::Instance instance) {
    VkSurfaceKHR c_surface;
    glfwCreateWindowSurface(instance, window_handle, nullptr, &c_surface);
    return vk::UniqueSurfaceKHR(c_surface, instance);
}
bool window_backend_glfw::is_close_requested() {
    return glfwWindowShouldClose(window_handle) != GLFW_FALSE;
}

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
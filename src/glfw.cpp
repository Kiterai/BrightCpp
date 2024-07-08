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

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
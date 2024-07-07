#include <brightcpp/internal/glfw.hpp>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

class glfw_initialization_handle : public system_module {
  public:
    glfw_initialization_handle() {
        BRIGHTCPP_GLFW_CHK_ERR(glfwInit());
    }

    ~glfw_initialization_handle() {
        glfwTerminate();
    }
};

std::unique_ptr<system_module> make_glfw_initializer() {
    return std::make_unique<glfw_initialization_handle>();
}

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
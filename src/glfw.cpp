#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <brightcpp/internal/glfw.hpp>
#include <iostream>
#include <vulkan/vulkan.hpp>

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define _LINE STRINGIZE(__LINE__)

#define BRIGHTCPP_GLFW_CHK_ERR(call)                                                                          \
    {                                                                                                         \
        call;                                                                                                 \
        if (const char *buf; glfwGetError(&buf) != GLFW_NO_ERROR) {                                           \
            throw std::runtime_error("error on " __FILE__ " line " _LINE ", " #call ": " + std::string(buf)); \
        }                                                                                                     \
    }

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

class window_backend_glfw : public window_backend {
    GLFWwindow *window_handle;

  public:
    window_backend_glfw(const window::settings &settings) {
        // COMMENTED OUT FOR ARCHITECTURE CHANGE

        // BRIGHTCPP_GLFW_CHK_ERR(glfwWindowHint(GLFW_RESIZABLE, current_settings.is_resizable ? GLFW_TRUE : GLFW_FALSE));
        // BRIGHTCPP_GLFW_CHK_ERR(glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API));
        // BRIGHTCPP_GLFW_CHK_ERR(glfw_window = glfwCreateWindow(current_settings.size.w, current_settings.size.h, current_settings.title.c_str(), NULL, NULL));
        // glfw_windows.insert(glfw_window);

        // internal::create_render_target(glfw_window);
        // internal::set_current_render_target(glfw_window);
    }
    ~window_backend_glfw() override {
        // COMMENTED OUT FOR ARCHITECTURE CHANGE

        // // destroy render target
        // glfw_windows.erase(glfw_window);
        // glfwDestroyWindow(glfw_window);
    }

    vk::UniqueSurfaceKHR get_vulkan_surface(vk::Instance instance) override {
        VkSurfaceKHR c_surface;
        glfwCreateWindowSurface(instance, window_handle, nullptr, &c_surface);
        return vk::UniqueSurfaceKHR(c_surface, instance);
    }
    bool is_close_requested() override {
        return glfwWindowShouldClose(window_handle) != GLFW_FALSE;
    }
};

class os_util_backend_glfw : public os_util_backend {
  public:
    os_util_backend_glfw();
    ~os_util_backend_glfw() override;

    std::unique_ptr<window_backend> create_window(const window::settings &settings) override;
};

os_util_backend_glfw::os_util_backend_glfw() {
    BRIGHTCPP_GLFW_CHK_ERR(glfwInit());
#ifdef _DEBUG
    std::cout << "GLFW initialized." << std::endl;
#endif
}

os_util_backend_glfw::~os_util_backend_glfw() {
#ifdef _DEBUG
    std::cout << "GLFW shutdown..." << std::endl;
#endif
    glfwTerminate();
}

std::unique_ptr<window_backend> os_util_backend_glfw::create_window(const window::settings &settings) {
    return std::make_unique<window_backend_glfw>();
}

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
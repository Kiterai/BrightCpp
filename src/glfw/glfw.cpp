#define GLFW_INCLUDE_VULKAN
#include "glfw.hpp"
#include "glfw_common.hpp"
#include <iostream>
#include <vulkan/vulkan.hpp>

BRIGHTCPP_OSUTIL_GLFW_START

class window_backend_glfw : public window_backend {
    GLFWwindow *window_handle;
    window::window_size window_size;

  public:
    window_backend_glfw(const window::settings &settings) {
        BRIGHTCPP_GLFW_CHK_ERR(glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API));
        if (settings.is_fullscreen) {
#ifdef _DEBUG
            std::cerr << "WARNING: In fullscreen mode, width and height settings are ignored. monitor size is used." << std::endl;
#endif
            // TODO
            auto monitor = glfwGetPrimaryMonitor();

            const auto mode = glfwGetVideoMode(monitor);
            window_size = {.w = mode->width, .h = mode->height};
            BRIGHTCPP_GLFW_CHK_ERR(glfwWindowHint(GLFW_DECORATED, GLFW_FALSE));
            BRIGHTCPP_GLFW_CHK_ERR(window_handle = glfwCreateWindow(window_size.w, window_size.h, settings.title.c_str(), NULL, NULL));

            int monitor_x, monitor_y;
            glfwGetMonitorPos(monitor, &monitor_x, &monitor_y);
            glfwSetWindowPos(window_handle, monitor_x, monitor_y);
        } else {
            window_size = settings.size;
            BRIGHTCPP_GLFW_CHK_ERR(window_handle = glfwCreateWindow(window_size.w, window_size.h, settings.title.c_str(), NULL, NULL));
        }
    }
    ~window_backend_glfw() override {
        glfwDestroyWindow(window_handle);
    }

    vk::UniqueSurfaceKHR get_vulkan_surface(vk::Instance instance) override {
        VkSurfaceKHR c_surface;
        glfwCreateWindowSurface(instance, window_handle, nullptr, &c_surface);
        return vk::UniqueSurfaceKHR(c_surface, instance);
    }
    bool is_close_requested() override {
        return glfwWindowShouldClose(window_handle) != GLFW_FALSE;
    }

    void set_size(int w, int h) override {
        BRIGHTCPP_GLFW_CHK_ERR(glfwSetWindowSize(window_handle, w, h));
    }
    std::tuple<int, int> get_size() const override {
        int w, h;
        BRIGHTCPP_GLFW_CHK_ERR(glfwGetWindowSize(window_handle, &w, &h));
        return {w, h};
    }

    void set_resizable(bool is_resizable) override {
        BRIGHTCPP_GLFW_CHK_ERR(glfwSetWindowAttrib(window_handle, GLFW_RESIZABLE, is_resizable ? GLFW_TRUE : GLFW_FALSE));
    }
    bool is_resizable() const override {
        int res;
        BRIGHTCPP_GLFW_CHK_ERR(res = glfwGetWindowAttrib(window_handle, GLFW_RESIZABLE));
        return res;
    }

    void set_fullscreen(bool is_fullscreen) override {
        throw std::runtime_error("not implemented set_fullscreen() with GLFW");
    }
    bool is_fullscreen() const override {
        throw std::runtime_error("not implemented is_fullscreen() with GLFW");
        return false;
    }

    void set_title(const std::string &title) override {
        BRIGHTCPP_GLFW_CHK_ERR(glfwSetWindowTitle(window_handle, title.c_str()));
    }
    std::string get_title() const override {
        const char *p;
        BRIGHTCPP_GLFW_CHK_ERR(p = glfwGetWindowTitle(window_handle));
        return std::string(p);
    }
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
    return std::make_unique<window_backend_glfw>(settings);
}

std::vector<const char *> os_util_backend_glfw::get_vulkan_required_instance_extensions() {
    std::vector<const char *> exts;

    uint32_t glfw_required_exts_count;
    const auto glfw_required_exts = glfwGetRequiredInstanceExtensions(&glfw_required_exts_count);
    for (const auto ext : std::span{glfw_required_exts, glfw_required_exts_count})
        exts.push_back(ext);

    return exts;
}

void os_util_backend_glfw::poll_events() {
    glfwPollEvents();
}

BRIGHTCPP_OSUTIL_GLFW_END

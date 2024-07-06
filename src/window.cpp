#include <GLFW/glfw3.h>
#include <brightcpp/window.hpp>
#include <stdexcept>
#include <unordered_set>

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

static size_t initialized_count = 0;
static std::unordered_set<GLFWwindow *> glfw_windows;

class glfw_initialization_handle {
  public:
    glfw_initialization_handle() {
        if (initialized_count == 0) {
            BRIGHTCPP_GLFW_CHK_ERR(glfwInit());
        }
        initialized_count++;
    }

    ~glfw_initialization_handle() {
        initialized_count--;
        if (initialized_count == 0) {
            glfwTerminate();
        }
    }
};

class window::_impl {
    glfw_initialization_handle gih;
    GLFWwindow *glfw_window;
    settings current_settings;

    std::function<void(window_size)> cb_on_resize;
    std::function<void()> cb_on_maximized;
    std::function<void()> cb_on_minimized;
    std::function<void()> cb_on_close;

  public:
    friend bool frame_update();

    _impl(const settings &initial_settings) : current_settings(initial_settings) {
        BRIGHTCPP_GLFW_CHK_ERR(glfwWindowHint(GLFW_RESIZABLE, current_settings.is_resizable ? GLFW_TRUE : GLFW_FALSE));
        BRIGHTCPP_GLFW_CHK_ERR(glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API));
        BRIGHTCPP_GLFW_CHK_ERR(glfw_window = glfwCreateWindow(current_settings.size.w, current_settings.size.h, current_settings.title.c_str(), NULL, NULL));
        glfw_windows.insert(glfw_window);
    }
    ~_impl() {
        glfw_windows.erase(glfw_window);
        glfwDestroyWindow(glfw_window);
    }

    void resize(window_size size) {
        BRIGHTCPP_GLFW_CHK_ERR(glfwSetWindowSize(glfw_window, size.w, size.h));
        current_settings.size = size;
    }
    window_size size() const {
        return current_settings.size;
    }

    void set_resizable(bool is_resizable) {
        BRIGHTCPP_GLFW_CHK_ERR(glfwWindowHint(GLFW_RESIZABLE, current_settings.is_resizable ? GLFW_TRUE : GLFW_FALSE));
        current_settings.is_resizable = is_resizable;
    }
    bool is_resizable() const {
        return current_settings.is_resizable;
    }

    void set_fullscreen(bool is_fullscreen) {
    }
    bool is_fullscreen() const {
        return current_settings.is_fullscreen;
    }

    void set_title(const std::string &title) {
        BRIGHTCPP_GLFW_CHK_ERR(glfwSetWindowTitle(glfw_window, title.c_str()));
        current_settings.title = title;
    }
    std::string title() const {
        return current_settings.title;
    }

    void on_resize(std::function<void(window_size)> &cb) {
        cb_on_resize = cb;
    }
    void on_minimized(std::function<void()> &cb) {
        cb_on_minimized = cb;
    }
    void on_maximized(std::function<void()> &cb) {
        cb_on_maximized = cb;
    }
    void on_close(std::function<void()> &cb) {
        cb_on_close = cb;
    }
};

window::window()
    : pimpl{std::make_unique<_impl>(settings{})} {}
window::window(const settings &initial_settings)
    : pimpl{std::make_unique<_impl>(initial_settings)} {}
window::~window() = default;

void window::resize(window_size size) { pimpl->resize(size); }
window::window_size window::size() const { return pimpl->size(); }

void window::set_resizable(bool is_resizable) { pimpl->set_resizable(is_resizable); }
bool window::is_resizable() const { return pimpl->is_resizable(); };

void window::set_fullscreen(bool is_fullscreen) { pimpl->set_fullscreen(is_fullscreen); }
bool window::is_fullscreen() const { return pimpl->is_fullscreen(); }

void window::set_title(std::string title) { pimpl->set_title(title); }
std::string window::title() const { return pimpl->title(); }

void window::on_resize(std::function<void(window_size)> &cb) { pimpl->on_resize(cb); }
void window::on_minimized(std::function<void()> &cb) { pimpl->on_minimized(cb); }
void window::on_maximized(std::function<void()> &cb) { pimpl->on_maximized(cb); }
void window::on_close(std::function<void()> &cb) { pimpl->on_close(cb); }

bool frame_update() {
    for (const auto glfw_window : glfw_windows) {
        glfwSwapBuffers(glfw_window);
    }
    glfwPollEvents();

    // one of windows closed, to finish application
    for (const auto glfw_window : glfw_windows) {
        if (glfwWindowShouldClose(glfw_window)) {
            return false;
        }
    }
    return true;
}

} // namespace BRIGHTCPP_NAMESPACE

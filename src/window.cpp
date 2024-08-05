#include <brightcpp/internal/system.hpp>
#include <brightcpp/window.hpp>
#include <stdexcept>
#include <unordered_set>

namespace BRIGHTCPP_NAMESPACE {

static size_t initialized_count = 0;

class window::_impl {
    [[no_unique_address]] internal::system_initializer _sys;
    settings current_settings;

  public:
    friend bool frame_update();

    _impl(const settings &initial_settings) : current_settings(initial_settings) {
    }
    ~_impl() {
    }

    void resize(window_size size) {
        // BRIGHTCPP_GLFW_CHK_ERR(glfwSetWindowSize(glfw_window, size.w, size.h));
        current_settings.size = size;
    }
    window_size size() const {
        return current_settings.size;
    }

    void set_resizable(bool is_resizable) {
        // BRIGHTCPP_GLFW_CHK_ERR(glfwWindowHint(GLFW_RESIZABLE, current_settings.is_resizable ? GLFW_TRUE : GLFW_FALSE));
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
        // BRIGHTCPP_GLFW_CHK_ERR(glfwSetWindowTitle(glfw_window, title.c_str()));
        current_settings.title = title;
    }
    std::string title() const {
        return current_settings.title;
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

bool frame_update() {
    // internal::apply_render();
    // for (const auto glfw_window : glfw_windows) {
    //     glfwSwapBuffers(glfw_window);
    // }
    // glfwPollEvents();

    // // one of windows closed, to finish application
    // for (const auto glfw_window : glfw_windows) {
    //     if (glfwWindowShouldClose(glfw_window)) {
    //         return false;
    //     }
    // }
    return true;
}

} // namespace BRIGHTCPP_NAMESPACE

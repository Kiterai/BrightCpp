#include "global_module.hpp"
#include "interfaces/graphics.hpp"
#include "interfaces/os_util.hpp"
#include "os/key/key.hpp"
#include <brightcpp/window.hpp>
#include <stdexcept>
#include <unordered_set>

BRIGHTCPP_START

using g_os_util = internal::global_module<internal::os_util_backend>;
using g_graphics = internal::global_module<internal::graphics_backend>;

namespace internal {

class window_impl {
    std::unique_ptr<internal::window_backend> window;
    rendertarget self_render_target;

  public:
    friend bool frame_update();

    window_impl(const window::settings &initial_settings)
        : window{g_os_util::get().create_window(initial_settings)},
          self_render_target{g_graphics::get().create_render_target(*window.get())} {}
    ~window_impl() {}

    rendertarget get_render_target() const {
        return self_render_target;
    }

    void resize(window::window_size size) {
        window->set_size(size.w, size.h);
    }
    window::window_size size() const {
        auto res = window->get_size();
        return window::window_size{
            .w = std::get<0>(res),
            .h = std::get<1>(res),
        };
    }

    void set_resizable(bool is_resizable) {
        window->set_resizable(is_resizable);
    }
    bool is_resizable() const {
        return window->is_resizable();
    }

    void set_fullscreen(bool is_fullscreen) {
        window->set_fullscreen(is_fullscreen);
    }
    bool is_fullscreen() const {
        return window->is_fullscreen();
    }

    void set_title(const std::string &title) {
        window->set_title(title);
    }
    std::string title() const {
        return window->get_title();
    }

    bool close_requested() const {
        return window->is_close_requested();
    }
};

handle_holder<window>::handle_value_t window_serial_id = 0;
std::unordered_map<handle_holder<window>::handle_value_t, window_impl> available_windows;

} // namespace internal

handle_holder<window>::handle_value_t register_window(const window::settings &settings) {
    const auto new_id = internal::window_serial_id;
    internal::window_serial_id++;

    internal::available_windows.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(new_id),
        std::forward_as_tuple(settings));
    return new_id;
}

void unregister_window(handle_holder<window>::handle_value_t id) {
    internal::available_windows.erase(id);
}

window::window()
    : handle_holder{register_window(settings{})} {}
window::window(const settings &initial_settings)
    : handle_holder{register_window(initial_settings)} {}
window::~window() = default;

void window::resize(window_size size) { internal::available_windows.at(handle()).resize(size); }
window::window_size window::size() const { return internal::available_windows.at(handle()).size(); }

rendertarget window::get_render_target() const { return internal::available_windows.at(handle()).get_render_target(); }

void window::set_resizable(bool is_resizable) { internal::available_windows.at(handle()).set_resizable(is_resizable); }
bool window::is_resizable() const { return internal::available_windows.at(handle()).is_resizable(); };

void window::set_fullscreen(bool is_fullscreen) { internal::available_windows.at(handle()).set_fullscreen(is_fullscreen); }
bool window::is_fullscreen() const { return internal::available_windows.at(handle()).is_fullscreen(); }

void window::set_title(std::string title) { internal::available_windows.at(handle()).set_title(title); }
std::string window::title() const { return internal::available_windows.at(handle()).title(); }

bool window::close_requested() const { return internal::available_windows.at(handle()).close_requested(); }

bool frame_update() {
    // internal::apply_render();
    g_os_util::get().poll_events();
    internal::global_module<internal::key_manager>::get().update();

    // one of windows closed, to finish application
    for (const auto &[id, available_window] : internal::available_windows) {
        if (available_window.close_requested()) {
            return false;
        }
    }
    return true;
}

BRIGHTCPP_END

#include "global_module.hpp"
#include "interfaces/graphics.hpp"
#include "interfaces/os_util.hpp"
#include "system.hpp"
#include <brightcpp/window.hpp>
#include <stdexcept>
#include <unordered_set>

namespace BRIGHTCPP_NAMESPACE {

static std::list<std::reference_wrapper<internal::window_backend>> available_windows;

using g_os_util = internal::global_module<internal::os_util_backend>;
using g_graphics = internal::global_module<internal::graphics_backend>;

class window::_impl {
    [[no_unique_address]] internal::system_initializer _sys;
    std::unique_ptr<internal::window_backend> window;
    render_target self_render_target;

    decltype(available_windows.begin()) wndlist_it;

  public:
    friend bool frame_update();

    _impl(const settings &initial_settings)
        : window{g_os_util::get().create_window(initial_settings)},
          self_render_target{g_graphics::get().create_render_target(*window.get())} {
        available_windows.push_front(*window.get());
        wndlist_it = available_windows.begin();
    }
    ~_impl() {
        available_windows.erase(wndlist_it);
    }

    render_target get_render_target() const {
        return self_render_target;
    }

    void resize(window_size size) {
        window->set_size(size.w, size.h);
    }
    window_size size() const {
        auto res = window->get_size();
        return window_size{
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

window::window()
    : pimpl{std::make_unique<_impl>(settings{})} {}
window::window(const settings &initial_settings)
    : pimpl{std::make_unique<_impl>(initial_settings)} {}
window::~window() = default;

void window::resize(window_size size) { pimpl->resize(size); }
window::window_size window::size() const { return pimpl->size(); }

render_target window::get_render_target() const { return pimpl->get_render_target(); }

void window::set_resizable(bool is_resizable) { pimpl->set_resizable(is_resizable); }
bool window::is_resizable() const { return pimpl->is_resizable(); };

void window::set_fullscreen(bool is_fullscreen) { pimpl->set_fullscreen(is_fullscreen); }
bool window::is_fullscreen() const { return pimpl->is_fullscreen(); }

void window::set_title(std::string title) { pimpl->set_title(title); }
std::string window::title() const { return pimpl->title(); }

bool window::close_requested() const { return pimpl->close_requested(); }

bool frame_update() {
    // internal::apply_render();
    g_os_util::get().poll_events();

    // one of windows closed, to finish application
    for (const auto &available_window : available_windows) {
        if (available_window.get().is_close_requested()) {
            return false;
        }
    }
    return true;
}

} // namespace BRIGHTCPP_NAMESPACE

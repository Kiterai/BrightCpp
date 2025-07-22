#include "../entity_holder.hpp"
#include "../global_module.hpp"
#include "../interfaces/os_util.hpp"
#include "../interfaces/rendertarget_factory.hpp"
#include "key/key.hpp"
#include <brightcpp/window.hpp>
#include <stdexcept>

BRIGHTCPP_START

using g_os_util = internal::global_module<internal::os_util_backend>;
using g_rt_factory = internal::global_module<internal::rendertarget_factory_backend>;

namespace internal {

class window_impl {
    std::unique_ptr<internal::window_backend> window;
    rendertarget self_render_target;

  public:
    friend bool frame_update();

    window_impl(const window::settings &initial_settings)
        : window{g_os_util::get().create_window(initial_settings)},
          self_render_target{g_rt_factory::get().create_render_target(*window.get())} {}
    ~window_impl() {}

    rendertarget get_render_target() const { return self_render_target; }

    void resize(window::window_size size) { window->set_size(size.w, size.h); }
    window::window_size size() const {
        auto res = window->get_size();
        return window::window_size{
            .w = std::get<0>(res),
            .h = std::get<1>(res),
        };
    }

    void set_resizable(bool is_resizable) { window->set_resizable(is_resizable); }
    bool is_resizable() const { return window->is_resizable(); }

    void set_fullscreen(bool is_fullscreen) { window->set_fullscreen(is_fullscreen); }
    bool is_fullscreen() const { return window->is_fullscreen(); }

    void set_title(const std::string &title) { window->set_title(title); }
    std::string title() const { return window->get_title(); }

    bool close_requested() const { return window->is_close_requested(); }
};

class window_manager : public entity_holder<window_impl> {
  public:
    bool is_closed_any() {
        for (const auto &[id, available_window] : db) {
            if (available_window.close_requested()) {
                return false;
            }
        }
        return true;
    }
};

std::optional<window_manager> wnd_backend;

template <> window_manager *global_module_constructor<window_manager>() {
    wnd_backend.emplace();
    return &*wnd_backend;
}

} // namespace internal

using g_wnd_manager_t = internal::global_module<internal::window_manager>;

window::window() : handle_holder{g_wnd_manager_t::get().make(settings{})} {}
window::window(const settings &initial_settings) : handle_holder{g_wnd_manager_t::get().make(initial_settings)} {}
window::~window() { g_wnd_manager_t::get().destroy(handle()); };

void window::resize(window_size size) { g_wnd_manager_t::get().get(handle()).resize(size); }
window::window_size window::size() const { return g_wnd_manager_t::get().get(handle()).size(); }

rendertarget window::get_render_target() const { return g_wnd_manager_t::get().get(handle()).get_render_target(); }

void window::set_resizable(bool is_resizable) { g_wnd_manager_t::get().get(handle()).set_resizable(is_resizable); }
bool window::is_resizable() const { return g_wnd_manager_t::get().get(handle()).is_resizable(); };

void window::set_fullscreen(bool is_fullscreen) { g_wnd_manager_t::get().get(handle()).set_fullscreen(is_fullscreen); }
bool window::is_fullscreen() const { return g_wnd_manager_t::get().get(handle()).is_fullscreen(); }

void window::set_title(std::string title) { g_wnd_manager_t::get().get(handle()).set_title(title); }
std::string window::title() const { return g_wnd_manager_t::get().get(handle()).title(); }

bool window::close_requested() const { return g_wnd_manager_t::get().get(handle()).close_requested(); }

bool frame_update() {
    // internal::apply_render();
    g_os_util::get().poll_events();
    internal::global_module<internal::key_manager>::get().update();

    return g_wnd_manager_t::get().is_closed_any();
}

BRIGHTCPP_END

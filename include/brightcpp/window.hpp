#pragma once

#include <brightcpp/common.hpp>
#include <functional>
#include <memory>
#include <string>

namespace BRIGHTCPP_NAMESPACE {

class window {
    class _impl;
    std::unique_ptr<_impl> pimpl;

  public:
    struct window_size {
        int w, h;
    };
    struct settings {
        window_size size = {640, 480};
        bool is_resizable = false;
        bool is_fullscreen = false;
        std::string title = "App";
    };

    window();
    window(const settings &initial_settings);
    ~window();

    void resize(window_size size);
    window_size size() const;

    void set_resizable(bool is_resizable);
    bool is_resizable() const;

    void set_fullscreen(bool is_fullscreen);
    bool is_fullscreen() const;

    void set_title(std::string title);
    std::string title() const;

    void on_resize(std::function<void(window_size)> &);
    void on_minimized(std::function<void()> &);
    void on_maximized(std::function<void()> &);
    void on_close(std::function<void()> &);
};

bool frame_update();

} // namespace BRIGHTCPP_NAMESPACE

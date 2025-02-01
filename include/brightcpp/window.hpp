#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/render_target.hpp>
#include <memory>
#include <string>

BRIGHTCPP_START

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

    render_target get_render_target() const;

    void resize(window_size size);
    window_size size() const;

    void set_resizable(bool is_resizable);
    bool is_resizable() const;

    void set_fullscreen(bool is_fullscreen);
    bool is_fullscreen() const;

    void set_title(std::string title);
    std::string title() const;

    bool close_requested() const;
};

bool frame_update();

BRIGHTCPP_END

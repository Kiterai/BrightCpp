#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/handle_holder.hpp>
#include <brightcpp/image_decl.hpp>
#include <brightcpp/geometry.hpp>
#include <brightcpp/rendertarget.hpp>
#include <string>

BRIGHTCPP_START

class image_clip : public handle_holder<image_impl> {
    int cx, cy, cw, ch;

  public:
    image_clip(handle_holder<image_impl>, int cx, int cy, int cw, int ch);
    ~image_clip();

    rect clipping_area() const;
    image_clip clip(int x, int y, int w, int h) const;
};

class image : public handle_holder<image_impl> {
    int w, h;

  public:
    image(const char *path);
    image(image &) = delete;
    ~image();

    rect_size size() const;
    image_clip clip() const;
    image_clip clip(int x, int y, int w, int h) const;

    rendertarget get_render_target() const;
};

BRIGHTCPP_END

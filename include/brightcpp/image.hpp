#pragma once

#include <brightcpp/common.hpp>
#include <functional>
#include <memory>
#include <string>

namespace BRIGHTCPP_NAMESPACE {

struct ratio {
    float value;

    template <class T>
    auto absolute(T whole) { return whole * this->value; }
};
constexpr ratio operator""_unit(long double value) {
    return ratio{static_cast<decltype(ratio::value)>(value)};
}
constexpr ratio operator""_percent(long double value) {
    return ratio{static_cast<decltype(ratio::value)>(value / 100.0l)};
}

struct pivot {
    ratio x, y;
};
namespace pivot_pos {

constexpr pivot left_top =
    {0.0_unit, 0.0_unit};
constexpr pivot left_center =
    {0.0_unit, 0.5_unit};
constexpr pivot left_bottom =
    {0.0_unit, 1.0_unit};

constexpr pivot center_top =
    {0.5_unit, 0.0_unit};
constexpr pivot center =
    {0.5_unit, 0.5_unit};
constexpr pivot center_bottom =
    {0.5_unit, 1.0_unit};

constexpr pivot right_top =
    {1.0_unit, 0.0_unit};
constexpr pivot right_center =
    {1.0_unit, 0.5_unit};
constexpr pivot right_bottom =
    {1.0_unit, 1.0_unit};

} // namespace pivot_pos

namespace internal {

class image_impl;

}

class image_clip {
    std::weak_ptr<internal::image_impl> p_impl;
    int cx, cy, cw, ch;

  public:
    image_clip(const std::weak_ptr<internal::image_impl> &p, int cx, int cy, int cw, int ch);
    ~image_clip();

    void draw(int x, int y);
    image_clip clip(int x, int y, int w, int h);
};

class image {
    std::shared_ptr<internal::image_impl> p_impl;

  public:
    image(const char* path);
    ~image();

    void draw(int x, int y);

    image_clip clip(int x, int y, int w, int h);
};

} // namespace BRIGHTCPP_NAMESPACE

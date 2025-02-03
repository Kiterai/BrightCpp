#pragma once

#include <brightcpp/common.hpp>

BRIGHTCPP_START

struct ratio {
    float value;

    template <class T>
    auto absolute(T whole) -> decltype(whole * this->value) { return whole * this->value; }
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

class point {
    int m_x, m_y;

  public:
    point(int x, int y) : m_x(x), m_y(y) {}
    int x() const { return m_x; }
    int y() const { return m_y; }
};

class rect_size {
    int m_w, m_h;

  public:
    rect_size(int w, int h) : m_w(w), m_h(h) {}
    int width() const { return m_w; }
    int height() const { return m_h; }
};

class rect {
    int m_x, m_y, m_w, m_h;

  public:
    rect(int left_x, int top_y, int w, int h) : m_x(left_x), m_y(top_y), m_w(w), m_h(h) {}
    int left() const { return m_x; }
    int right() const { return m_x + m_w; }
    int top() const { return m_y; }
    int bottom() const { return m_y + m_h; }
    point left_top() const { return {m_x, m_y}; }
    point right_top() const { return {m_x + m_w, m_y}; }
    point left_bottom() const { return {m_x, m_y + m_h}; }
    point right_bottom() const { return {m_x + m_w, m_y + m_h}; }

    rect_size size() const { return {m_w, m_h}; }
    int width() const { return m_w; }
    int height() const { return m_h; }
};

BRIGHTCPP_END

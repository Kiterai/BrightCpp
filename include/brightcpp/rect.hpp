#pragma once

#include <brightcpp/common.hpp>

namespace BRIGHTCPP_NAMESPACE {

class rect {
    int m_x, m_y, m_w, m_h;

  public:
    rect(int left_x, int top_y, int w, int h) : m_x(left_x), m_y(top_y), m_w(w), m_h(h) {}
    int left() const { return m_x; }
    int right() const { return m_x + m_w; }
    int top() const { return m_y; }
    int bottom() const { return m_y + m_h; }
    int width() const { return m_w; }
    int height() const { return m_h; }
};

} // namespace BRIGHTCPP_NAMESPACE

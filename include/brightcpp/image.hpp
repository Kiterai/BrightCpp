#pragma once

#include <brightcpp/common.hpp>
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

struct anchor {
    ratio x, y;
};
namespace anchor_pos {

constexpr anchor left_top =
    {0.0_unit, 0.0_unit};
constexpr anchor left_center =
    {0.0_unit, 0.5_unit};
constexpr anchor left_bottom =
    {0.0_unit, 1.0_unit};

constexpr anchor center_top =
    {0.5_unit, 0.0_unit};
constexpr anchor center =
    {0.5_unit, 0.5_unit};
constexpr anchor center_bottom =
    {0.5_unit, 1.0_unit};

constexpr anchor right_top =
    {1.0_unit, 0.0_unit};
constexpr anchor right_center =
    {1.0_unit, 0.5_unit};
constexpr anchor right_bottom =
    {1.0_unit, 1.0_unit};

} // namespace anchor_pos

class image {
  private:
    class _impl;
    std::unique_ptr<_impl> pimpl;

  public:
    class image_clip {
      private:
      public:
        image_clip(/* args */);
        ~image_clip();
    };

    image();
    ~image();
};

} // namespace BRIGHTCPP_NAMESPACE

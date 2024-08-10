#pragma once

#include <brightcpp/common.hpp>

namespace BRIGHTCPP_NAMESPACE {

template <class T>
class handle_holder {
    using handle_value_t = size_t;

    handle_value_t handle_value;

  protected:
    handle_holder(handle_value_t _handle) : handle_value(_handle) {}

  public:
    handle_value_t handle() const { return handle_value; }
};

} // namespace BRIGHTCPP_NAMESPACE

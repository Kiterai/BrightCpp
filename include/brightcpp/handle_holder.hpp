#pragma once

#include <brightcpp/common.hpp>
#include <cstddef>

namespace BRIGHTCPP_NAMESPACE {

template <class T>
class handle_holder {
  public:
    using handle_value_t = std::size_t;
    handle_value_t handle() const { return handle_value; }

  private:
    handle_value_t handle_value;

  protected:
    handle_holder(handle_value_t _handle) : handle_value(_handle) {}
};

} // namespace BRIGHTCPP_NAMESPACE

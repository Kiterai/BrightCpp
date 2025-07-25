#pragma once

#include <brightcpp/common.hpp>
#include <cstddef>

BRIGHTCPP_START

using entity_handle_t = std::size_t;

template <class T>
class handle_holder {
  public:
    using handle_value_t = entity_handle_t;
    handle_value_t handle() const { return handle_value; }

  private:
    handle_value_t handle_value;

  protected:
    handle_holder(handle_value_t _handle) : handle_value(_handle) {}
};

BRIGHTCPP_END

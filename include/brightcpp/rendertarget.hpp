#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/handle_holder.hpp>

BRIGHTCPP_START

class rendertarget : public handle_holder<rendertarget> {
  public:
    rendertarget(handle_holder<rendertarget>::handle_value_t);
};

BRIGHTCPP_END

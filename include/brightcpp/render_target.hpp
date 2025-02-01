#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/handle_holder.hpp>

BRIGHTCPP_START

class render_target : public handle_holder<render_target> {
  public:
    render_target(handle_holder<render_target>::handle_value_t);
};

BRIGHTCPP_END

#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/handle_holder.hpp>

namespace BRIGHTCPP_NAMESPACE {

class render_target : handle_holder<render_target> {
  public:
    render_target(handle_holder<render_target>::handle_value_t);
};

} // namespace BRIGHTCPP_NAMESPACE

#pragma once

#include <brightcpp/common.hpp>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

class system_initializer {
  public:
    system_initializer();
    ~system_initializer();
};

void system_init();
void system_deinit();

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

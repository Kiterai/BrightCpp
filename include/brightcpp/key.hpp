#pragma once

#include <brightcpp/common.hpp>

BRIGHTCPP_START

class key {
  public:
    bool pressed() const;
    bool released() const;
    bool pressing() const;
};

BRIGHTCPP_END

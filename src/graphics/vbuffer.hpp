#pragma once

#include "../interfaces/vbuffer_factory.hpp"
#include <brightcpp/common.hpp>
#include <brightcpp/handle_holder.hpp>

BRIGHTCPP_START

namespace internal {

class vbuffer : public handle_holder<vbuffer_impl> {
  public:
    vbuffer(size_t bytes_num);
    ~vbuffer();
};

} // namespace internal

BRIGHTCPP_END

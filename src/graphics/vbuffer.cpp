#include "vbuffer.hpp"
#include "../global_module.hpp"

BRIGHTCPP_START

namespace internal {

using g_vbuffer = global_module<vbuffer_factory_backend>;

vbuffer::vbuffer(size_t bytes_num) : handle_holder(g_vbuffer::get().make(bytes_num)) {}
vbuffer::~vbuffer() {}

} // namespace internal

BRIGHTCPP_END

#pragma once

#include <brightcpp/common.hpp>
#include <unordered_map>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

template <class T>
class global_module {
    static inline T* ref;

  public:
    static void set(T &r) { ref = &r; }
    static T &get() { return *ref; };
};

class system_initializer {
  public:
    system_initializer();
    ~system_initializer();
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE

#pragma once

#include <brightcpp/common.hpp>

BRIGHTCPP_START

namespace internal {

template <class T>
T *global_module_constructor();

template <class T>
class global_module {
    static inline T *ref = nullptr;

  public:
    static void set(T &r) { ref = &r; }
    static T &get() {
        if (ref == nullptr)
            ref = global_module_constructor<T>();
        return *ref;
    };
};

} // namespace internal

BRIGHTCPP_END

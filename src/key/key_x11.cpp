#ifdef __unix__

#include "../global_module.hpp"
#include "../interfaces/os_util.hpp"

BRIGHTCPP_START

namespace internal {

class x11_key_backend : public key_backend {
  public:
    bool get_key_state(int key) const override {
        // TODO
        return false;
    }
};

x11_key_backend kbbackend;

template <>
key_backend *global_module_constructor<key_backend>() {
    return &kbbackend;
}

} // namespace internal

BRIGHTCPP_END

#endif

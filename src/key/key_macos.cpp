#ifdef __APPLE__

#include "../global_module.hpp"
#include "../interfaces/os_util.hpp"
#include <ApplicationServices/ApplicationServices.h>

BRIGHTCPP_START

namespace internal {

class macos_key_backend : public key_backend {
  public:
    bool get_key_state(int key) const override {
        return CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, key);
    }
};

macos_key_backend kbbackend;

template <>
key_backend *global_module_constructor<key_backend>() {
    return &kbbackend;
}

} // namespace internal

BRIGHTCPP_END

#endif

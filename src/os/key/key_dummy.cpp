#include "../../global_module.hpp"
#include "key.hpp"

BRIGHTCPP_START

namespace internal {

class dummy_key_backend : public key_backend {
  public:
    bool get_key_state(int key) const override {
        return 0;
    }
    int get_code_by_id(key_id id) const override {
        return 0;
    }
};

dummy_key_backend kbbackend;

template <>
key_backend *global_module_constructor<key_backend>() {
    return &kbbackend;
}

} // namespace internal

BRIGHTCPP_END

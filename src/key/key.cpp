#include "../global_module.hpp"
#include "../interfaces/os_util.hpp"
#include <brightcpp/key.hpp>

BRIGHTCPP_START

key __key_factory(int code) {
    return key{code};
}

key key_a = __key_factory('A');

key::key(int code) : code{code} {}

bool key::pressed() const {
    return internal::global_module<internal::key_backend>::get().get_key_state(code);
}
bool key::released() const {
    // TODO
    throw std::exception("not implemented");
}
bool key::pressing() const {
    // TODO
    throw std::exception("not implemented");
}

BRIGHTCPP_END

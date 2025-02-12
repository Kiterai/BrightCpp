#include "../global_module.hpp"
#include "../interfaces/os_util.hpp"
#include <brightcpp/key.hpp>

BRIGHTCPP_START

key __key_factory(int code) {
    return key{code};
}

key key_a = __key_factory('A');
key key_b = __key_factory('B');
key key_c = __key_factory('C');
key key_d = __key_factory('D');
key key_e = __key_factory('E');
key key_f = __key_factory('F');
key key_g = __key_factory('G');
key key_h = __key_factory('H');
key key_i = __key_factory('I');
key key_j = __key_factory('J');
key key_k = __key_factory('K');
key key_l = __key_factory('L');
key key_m = __key_factory('M');
key key_n = __key_factory('N');
key key_o = __key_factory('O');
key key_p = __key_factory('P');
key key_q = __key_factory('Q');
key key_r = __key_factory('R');
key key_s = __key_factory('S');
key key_t = __key_factory('T');
key key_u = __key_factory('U');
key key_v = __key_factory('V');
key key_w = __key_factory('W');
key key_x = __key_factory('X');
key key_y = __key_factory('Y');
key key_z = __key_factory('Z');
key key_0 = __key_factory('0');
key key_1 = __key_factory('1');
key key_2 = __key_factory('2');
key key_3 = __key_factory('3');
key key_4 = __key_factory('4');
key key_5 = __key_factory('5');
key key_6 = __key_factory('6');
key key_7 = __key_factory('7');
key key_8 = __key_factory('8');
key key_9 = __key_factory('9');
// TODO: This is just Windows Virtual Keycode
key key_lshift = __key_factory(0xA0);
key key_rshift = __key_factory(0xA1);
key key_lctrl = __key_factory(0xA2);
key key_rctrl = __key_factory(0xA3);
key key_lalt = __key_factory(0xA4);
key key_ralt = __key_factory(0xA5);
key key_space = __key_factory(' ');
key key_enter = __key_factory('\r');
key key_esc = __key_factory(0x1B);
key key_left = __key_factory(0x25);
key key_up = __key_factory(0x26);
key key_right = __key_factory(0x27);
key key_down = __key_factory(0x28);

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

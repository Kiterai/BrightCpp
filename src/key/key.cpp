#include "key.hpp"
#include "../global_module.hpp"
#include "../interfaces/os_util.hpp"
#include <brightcpp/key.hpp>

BRIGHTCPP_START

namespace internal {

key_manager g_key_manager;

template <>
key_manager *global_module_constructor<key_manager>() {
    return &g_key_manager;
}

key key_manager::register_key(int code) {
    key_list.push_back(code);
    return key{code};
}

void key_manager::update() {
    const auto &g = global_module<key_backend>::get();

    for (const auto tgt : key_list) {
        bool pressing = g.get_key_state(tgt);
        auto old_state = keys[tgt];
        keys[tgt] = key_state{
            .pressing = pressing,
            .pressed = !old_state.pressing && pressing,
            .released = old_state.pressing && !pressing,
        };
    }
}

key_manager::key_state key_manager::get_keystate(key _key) const {
    return keys[_key.code];
}

} // namespace internal

key key_a = internal::g_key_manager.register_key('A');
key key_b = internal::g_key_manager.register_key('B');
key key_c = internal::g_key_manager.register_key('C');
key key_d = internal::g_key_manager.register_key('D');
key key_e = internal::g_key_manager.register_key('E');
key key_f = internal::g_key_manager.register_key('F');
key key_g = internal::g_key_manager.register_key('G');
key key_h = internal::g_key_manager.register_key('H');
key key_i = internal::g_key_manager.register_key('I');
key key_j = internal::g_key_manager.register_key('J');
key key_k = internal::g_key_manager.register_key('K');
key key_l = internal::g_key_manager.register_key('L');
key key_m = internal::g_key_manager.register_key('M');
key key_n = internal::g_key_manager.register_key('N');
key key_o = internal::g_key_manager.register_key('O');
key key_p = internal::g_key_manager.register_key('P');
key key_q = internal::g_key_manager.register_key('Q');
key key_r = internal::g_key_manager.register_key('R');
key key_s = internal::g_key_manager.register_key('S');
key key_t = internal::g_key_manager.register_key('T');
key key_u = internal::g_key_manager.register_key('U');
key key_v = internal::g_key_manager.register_key('V');
key key_w = internal::g_key_manager.register_key('W');
key key_x = internal::g_key_manager.register_key('X');
key key_y = internal::g_key_manager.register_key('Y');
key key_z = internal::g_key_manager.register_key('Z');
key key_0 = internal::g_key_manager.register_key('0');
key key_1 = internal::g_key_manager.register_key('1');
key key_2 = internal::g_key_manager.register_key('2');
key key_3 = internal::g_key_manager.register_key('3');
key key_4 = internal::g_key_manager.register_key('4');
key key_5 = internal::g_key_manager.register_key('5');
key key_6 = internal::g_key_manager.register_key('6');
key key_7 = internal::g_key_manager.register_key('7');
key key_8 = internal::g_key_manager.register_key('8');
key key_9 = internal::g_key_manager.register_key('9');
// TODO: This is just Windows Virtual Keycode
key key_lshift = internal::g_key_manager.register_key(0xA0);
key key_rshift = internal::g_key_manager.register_key(0xA1);
key key_lctrl = internal::g_key_manager.register_key(0xA2);
key key_rctrl = internal::g_key_manager.register_key(0xA3);
key key_lalt = internal::g_key_manager.register_key(0xA4);
key key_ralt = internal::g_key_manager.register_key(0xA5);
key key_space = internal::g_key_manager.register_key(' ');
key key_enter = internal::g_key_manager.register_key('\r');
key key_esc = internal::g_key_manager.register_key(0x1B);
key key_left = internal::g_key_manager.register_key(0x25);
key key_up = internal::g_key_manager.register_key(0x26);
key key_right = internal::g_key_manager.register_key(0x27);
key key_down = internal::g_key_manager.register_key(0x28);

key::key(int code) : code{code} {}

bool key::pressed() const {
    return internal::global_module<internal::key_manager>::get().get_keystate(*this).pressed;
}
bool key::released() const {
    return internal::global_module<internal::key_manager>::get().get_keystate(*this).released;
}
bool key::pressing() const {
    return internal::global_module<internal::key_manager>::get().get_keystate(*this).pressing;
}

BRIGHTCPP_END

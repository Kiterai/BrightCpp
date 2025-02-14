#include "key.hpp"
#include "../global_module.hpp"
#include "../interfaces/os_util.hpp"
#include <brightcpp/key.hpp>
#include <iostream>

BRIGHTCPP_START

namespace internal {

key_manager g_key_manager;

template <>
key_manager *global_module_constructor<key_manager>() {
    return &g_key_manager;
}

key key_manager::register_key(key_id id) {
    auto &g = global_module<key_backend>::get();
    int code = g.get_code_by_id(id);
    key_list.push_back(code);
    return key{code};
}

void key_manager::update() {
    auto &g = global_module<key_backend>::get();
    g.update();
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

key key_a = internal::g_key_manager.register_key(internal::key_id::A);
key key_b = internal::g_key_manager.register_key(internal::key_id::B);
key key_c = internal::g_key_manager.register_key(internal::key_id::C);
key key_d = internal::g_key_manager.register_key(internal::key_id::D);
key key_e = internal::g_key_manager.register_key(internal::key_id::E);
key key_f = internal::g_key_manager.register_key(internal::key_id::F);
key key_g = internal::g_key_manager.register_key(internal::key_id::G);
key key_h = internal::g_key_manager.register_key(internal::key_id::H);
key key_i = internal::g_key_manager.register_key(internal::key_id::I);
key key_j = internal::g_key_manager.register_key(internal::key_id::J);
key key_k = internal::g_key_manager.register_key(internal::key_id::K);
key key_l = internal::g_key_manager.register_key(internal::key_id::L);
key key_m = internal::g_key_manager.register_key(internal::key_id::M);
key key_n = internal::g_key_manager.register_key(internal::key_id::N);
key key_o = internal::g_key_manager.register_key(internal::key_id::O);
key key_p = internal::g_key_manager.register_key(internal::key_id::P);
key key_q = internal::g_key_manager.register_key(internal::key_id::Q);
key key_r = internal::g_key_manager.register_key(internal::key_id::R);
key key_s = internal::g_key_manager.register_key(internal::key_id::S);
key key_t = internal::g_key_manager.register_key(internal::key_id::T);
key key_u = internal::g_key_manager.register_key(internal::key_id::U);
key key_v = internal::g_key_manager.register_key(internal::key_id::V);
key key_w = internal::g_key_manager.register_key(internal::key_id::W);
key key_x = internal::g_key_manager.register_key(internal::key_id::X);
key key_y = internal::g_key_manager.register_key(internal::key_id::Y);
key key_z = internal::g_key_manager.register_key(internal::key_id::Z);
key key_0 = internal::g_key_manager.register_key(internal::key_id::Key0);
key key_1 = internal::g_key_manager.register_key(internal::key_id::Key1);
key key_2 = internal::g_key_manager.register_key(internal::key_id::Key2);
key key_3 = internal::g_key_manager.register_key(internal::key_id::Key3);
key key_4 = internal::g_key_manager.register_key(internal::key_id::Key4);
key key_5 = internal::g_key_manager.register_key(internal::key_id::Key5);
key key_6 = internal::g_key_manager.register_key(internal::key_id::Key6);
key key_7 = internal::g_key_manager.register_key(internal::key_id::Key7);
key key_8 = internal::g_key_manager.register_key(internal::key_id::Key8);
key key_9 = internal::g_key_manager.register_key(internal::key_id::Key9);
key key_lshift = internal::g_key_manager.register_key(internal::key_id::LShift);
key key_rshift = internal::g_key_manager.register_key(internal::key_id::RShift);
key key_lctrl = internal::g_key_manager.register_key(internal::key_id::LCtrl);
key key_rctrl = internal::g_key_manager.register_key(internal::key_id::RCtrl);
key key_lalt = internal::g_key_manager.register_key(internal::key_id::LAlt);
key key_ralt = internal::g_key_manager.register_key(internal::key_id::RAlt);
key key_space = internal::g_key_manager.register_key(internal::key_id::Space);
key key_enter = internal::g_key_manager.register_key(internal::key_id::Enter);
key key_esc = internal::g_key_manager.register_key(internal::key_id::Esc);
key key_left = internal::g_key_manager.register_key(internal::key_id::Left);
key key_up = internal::g_key_manager.register_key(internal::key_id::Up);
key key_right = internal::g_key_manager.register_key(internal::key_id::Right);
key key_down = internal::g_key_manager.register_key(internal::key_id::Down);

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

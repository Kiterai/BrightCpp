#ifdef __unix__

#include "../global_module.hpp"
#include "key.hpp"
#include <X11/Xlib.h>
#define XK_LATIN1
#define XK_MISCELLANY
#include <X11/keysymdef.h>
#include <bitset>
#include <iostream>
#include <optional>

BRIGHTCPP_START

namespace internal {

class x11_key_backend : public key_backend {
    Display *disp;
    char keys[32];
    std::unordered_map<key_id, int> id_code_map;

  public:
    x11_key_backend() {
        disp = XOpenDisplay(nullptr);

        id_code_map = {
            {key_id::A, XKeysymToKeycode(disp, XK_a)},
            {key_id::B, XKeysymToKeycode(disp, XK_b)},
            {key_id::C, XKeysymToKeycode(disp, XK_c)},
            {key_id::D, XKeysymToKeycode(disp, XK_d)},
            {key_id::E, XKeysymToKeycode(disp, XK_e)},
            {key_id::F, XKeysymToKeycode(disp, XK_f)},
            {key_id::G, XKeysymToKeycode(disp, XK_g)},
            {key_id::H, XKeysymToKeycode(disp, XK_h)},
            {key_id::I, XKeysymToKeycode(disp, XK_i)},
            {key_id::J, XKeysymToKeycode(disp, XK_j)},
            {key_id::K, XKeysymToKeycode(disp, XK_k)},
            {key_id::L, XKeysymToKeycode(disp, XK_l)},
            {key_id::M, XKeysymToKeycode(disp, XK_m)},
            {key_id::N, XKeysymToKeycode(disp, XK_n)},
            {key_id::O, XKeysymToKeycode(disp, XK_o)},
            {key_id::P, XKeysymToKeycode(disp, XK_p)},
            {key_id::Q, XKeysymToKeycode(disp, XK_q)},
            {key_id::R, XKeysymToKeycode(disp, XK_r)},
            {key_id::S, XKeysymToKeycode(disp, XK_s)},
            {key_id::T, XKeysymToKeycode(disp, XK_t)},
            {key_id::U, XKeysymToKeycode(disp, XK_u)},
            {key_id::V, XKeysymToKeycode(disp, XK_v)},
            {key_id::W, XKeysymToKeycode(disp, XK_w)},
            {key_id::X, XKeysymToKeycode(disp, XK_x)},
            {key_id::Y, XKeysymToKeycode(disp, XK_y)},
            {key_id::Z, XKeysymToKeycode(disp, XK_z)},
            {key_id::Key0, XKeysymToKeycode(disp, XK_0)},
            {key_id::Key1, XKeysymToKeycode(disp, XK_1)},
            {key_id::Key2, XKeysymToKeycode(disp, XK_2)},
            {key_id::Key3, XKeysymToKeycode(disp, XK_3)},
            {key_id::Key4, XKeysymToKeycode(disp, XK_4)},
            {key_id::Key5, XKeysymToKeycode(disp, XK_5)},
            {key_id::Key6, XKeysymToKeycode(disp, XK_6)},
            {key_id::Key7, XKeysymToKeycode(disp, XK_7)},
            {key_id::Key8, XKeysymToKeycode(disp, XK_8)},
            {key_id::Key9, XKeysymToKeycode(disp, XK_9)},
            {key_id::LShift, XKeysymToKeycode(disp, XK_Shift_L)},
            {key_id::RShift, XKeysymToKeycode(disp, XK_Shift_R)},
            {key_id::LCtrl, XKeysymToKeycode(disp, XK_Control_L)},
            {key_id::RCtrl, XKeysymToKeycode(disp, XK_Control_R)},
            {key_id::LAlt, XKeysymToKeycode(disp, XK_Alt_L)},
            {key_id::RAlt, XKeysymToKeycode(disp, XK_Alt_R)},
            {key_id::Space, XKeysymToKeycode(disp, XK_space)},
            {key_id::Enter, XKeysymToKeycode(disp, XK_Return)},
            {key_id::Esc, XKeysymToKeycode(disp, XK_Escape)},
            {key_id::Left, XKeysymToKeycode(disp, XK_Left)},
            {key_id::Up, XKeysymToKeycode(disp, XK_Up)},
            {key_id::Right, XKeysymToKeycode(disp, XK_Right)},
            {key_id::Down, XKeysymToKeycode(disp, XK_Down)},
        };
    }
    void update() override {
        XQueryKeymap(disp, keys);
    }
    bool get_key_state(int key) const override {
        return keys[key / 8] & (1 << (key % 8));
    }
    int get_code_by_id(key_id id) const override {
        return id_code_map.at(id);
    }
};

std::optional<x11_key_backend> kbbackend;

template <>
key_backend *global_module_constructor<key_backend>() {
    kbbackend.emplace();
    return &*kbbackend;
}

} // namespace internal

BRIGHTCPP_END

#endif

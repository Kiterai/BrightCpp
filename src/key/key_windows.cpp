#ifdef _WIN32

#include "../global_module.hpp"
#include "key.hpp"
#include <Windows.h>
#include <unordered_map>

BRIGHTCPP_START

namespace internal {

class windows_key_backend : public key_backend {
  public:
    bool get_key_state(int key) const override {
        return GetAsyncKeyState(key);
    }
    int get_code_by_id(key_id id) const override {
        static std::unordered_map<key_id, int> id_code_map = {
            {key_id::A, 'A'},
            {key_id::B, 'B'},
            {key_id::C, 'C'},
            {key_id::D, 'D'},
            {key_id::E, 'E'},
            {key_id::F, 'F'},
            {key_id::G, 'G'},
            {key_id::H, 'H'},
            {key_id::I, 'I'},
            {key_id::J, 'J'},
            {key_id::K, 'K'},
            {key_id::L, 'L'},
            {key_id::M, 'M'},
            {key_id::N, 'N'},
            {key_id::O, 'O'},
            {key_id::P, 'P'},
            {key_id::Q, 'Q'},
            {key_id::R, 'R'},
            {key_id::S, 'S'},
            {key_id::T, 'T'},
            {key_id::U, 'U'},
            {key_id::V, 'V'},
            {key_id::W, 'W'},
            {key_id::X, 'X'},
            {key_id::Y, 'Y'},
            {key_id::Z, 'Z'},
            {key_id::Key0, '0'},
            {key_id::Key1, '1'},
            {key_id::Key2, '2'},
            {key_id::Key3, '3'},
            {key_id::Key4, '4'},
            {key_id::Key5, '5'},
            {key_id::Key6, '6'},
            {key_id::Key7, '7'},
            {key_id::Key8, '8'},
            {key_id::Key9, '9'},
            {key_id::LShift, VK_LSHIFT},
            {key_id::RShift, VK_RSHIFT},
            {key_id::LCtrl, VK_LCONTROL},
            {key_id::RCtrl, VK_RCONTROL},
            {key_id::LAlt, VK_LMENU},
            {key_id::RAlt, VK_RMENU},
            {key_id::Space, VK_SPACE},
            {key_id::Enter, VK_RETURN},
            {key_id::Esc, VK_ESCAPE},
            {key_id::Left, VK_LEFT},
            {key_id::Up, VK_UP},
            {key_id::Right, VK_RIGHT},
            {key_id::Down, VK_DOWN},
        };
        return id_code_map.at(id);
    }
};

windows_key_backend kbbackend;

template <>
key_backend *global_module_constructor<key_backend>() {
    return &kbbackend;
}

} // namespace internal

BRIGHTCPP_END

#endif

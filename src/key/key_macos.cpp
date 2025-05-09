#ifdef __APPLE__

#if !TARGET_OS_IOS

#include "../global_module.hpp"
#include "key.hpp"
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

BRIGHTCPP_START

namespace internal {

class macos_key_backend : public key_backend {
  public:
    bool get_key_state(int key) const override {
        return CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, key);
    }
    int get_code_by_id(key_id id) const override {
        static std::unordered_map<key_id, int> id_code_map = {
            {key_id::A, kVK_ANSI_A},
            {key_id::B, kVK_ANSI_B},
            {key_id::C, kVK_ANSI_C},
            {key_id::D, kVK_ANSI_D},
            {key_id::E, kVK_ANSI_E},
            {key_id::F, kVK_ANSI_F},
            {key_id::G, kVK_ANSI_G},
            {key_id::H, kVK_ANSI_H},
            {key_id::I, kVK_ANSI_I},
            {key_id::J, kVK_ANSI_J},
            {key_id::K, kVK_ANSI_K},
            {key_id::L, kVK_ANSI_L},
            {key_id::M, kVK_ANSI_M},
            {key_id::N, kVK_ANSI_N},
            {key_id::O, kVK_ANSI_O},
            {key_id::P, kVK_ANSI_P},
            {key_id::Q, kVK_ANSI_Q},
            {key_id::R, kVK_ANSI_R},
            {key_id::S, kVK_ANSI_S},
            {key_id::T, kVK_ANSI_T},
            {key_id::U, kVK_ANSI_U},
            {key_id::V, kVK_ANSI_V},
            {key_id::W, kVK_ANSI_W},
            {key_id::X, kVK_ANSI_X},
            {key_id::Y, kVK_ANSI_Y},
            {key_id::Z, kVK_ANSI_Z},
            {key_id::Key0, kVK_ANSI_0},
            {key_id::Key1, kVK_ANSI_1},
            {key_id::Key2, kVK_ANSI_2},
            {key_id::Key3, kVK_ANSI_3},
            {key_id::Key4, kVK_ANSI_4},
            {key_id::Key5, kVK_ANSI_5},
            {key_id::Key6, kVK_ANSI_6},
            {key_id::Key7, kVK_ANSI_7},
            {key_id::Key8, kVK_ANSI_8},
            {key_id::Key9, kVK_ANSI_9},
            {key_id::LShift, kVK_Shift},
            {key_id::RShift, kVK_RightShift},
            {key_id::LCtrl, kVK_Control},
            {key_id::RCtrl, kVK_RightControl},
            {key_id::LAlt, kVK_Option},
            {key_id::RAlt, kVK_RightOption},
            {key_id::Space, kVK_Space},
            {key_id::Enter, kVK_Return},
            {key_id::Esc, kVK_Escape},
            {key_id::Left, kVK_LeftArrow},
            {key_id::Up, kVK_UpArrow},
            {key_id::Right, kVK_RightArrow},
            {key_id::Down, kVK_DownArrow},
        };
        return id_code_map.at(id);
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

#endif

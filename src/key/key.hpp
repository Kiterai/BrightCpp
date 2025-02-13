#pragma once

#include <array>
#include <brightcpp/common.hpp>
#include <brightcpp/key.hpp>
#include <vector>

BRIGHTCPP_START

namespace internal {

enum class key_id {
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    Key0,
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,
    LShift,
    RShift,
    LCtrl,
    RCtrl,
    LAlt,
    RAlt,
    Space,
    Enter,
    Esc,
    Left,
    Up,
    Right,
    Down,
};

class key_backend {
  public:
    virtual int get_code_by_id(key_id id) const = 0;
    virtual bool get_key_state(int key) const = 0;
};

class key_manager {
  public:
    struct key_state {
        bool pressing = false, pressed = false, released = false;
    };

  private:
    std::array<key_state, 256> keys;
    std::vector<int> key_list;

  public:
    key register_key(key_id id);
    void update();
    key_manager::key_state get_keystate(key _key) const;
};

} // namespace internal

BRIGHTCPP_END

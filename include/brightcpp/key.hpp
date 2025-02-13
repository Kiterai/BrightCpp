#pragma once

#include <brightcpp/common.hpp>

BRIGHTCPP_START

namespace internal {
class key_manager;
};

class key {
    const int code;
    key(int);

  public:
    friend class internal::key_manager;

    bool pressed() const;
    bool released() const;
    bool pressing() const;
};

extern key key_0;
extern key key_1;
extern key key_2;
extern key key_3;
extern key key_4;
extern key key_5;
extern key key_6;
extern key key_7;
extern key key_8;
extern key key_9;
extern key key_a;
extern key key_b;
extern key key_c;
extern key key_d;
extern key key_e;
extern key key_f;
extern key key_g;
extern key key_h;
extern key key_i;
extern key key_j;
extern key key_k;
extern key key_l;
extern key key_m;
extern key key_n;
extern key key_o;
extern key key_p;
extern key key_q;
extern key key_r;
extern key key_s;
extern key key_t;
extern key key_u;
extern key key_v;
extern key key_w;
extern key key_x;
extern key key_y;
extern key key_z;
extern key key_lshift;
extern key key_rshift;
extern key key_lctrl;
extern key key_rctrl;
extern key key_lalt;
extern key key_ralt;
extern key key_space;
extern key key_enter;
extern key key_esc;
extern key key_left;
extern key key_up;
extern key key_right;
extern key key_down;

BRIGHTCPP_END

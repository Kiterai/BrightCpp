#include <array>
#include <brightcpp/common.hpp>
#include <brightcpp/key.hpp>
#include <vector>

BRIGHTCPP_START

namespace internal {

class key_manager {
  public:
    struct key_state {
        bool pressing = false, pressed = false, released = false;
    };

  private:
    std::array<key_state, 256> keys;
    std::vector<int> key_list;

  public:
    key register_key(int code);
    void update();
    key_manager::key_state get_keystate(key _key) const;
};

} // namespace internal

BRIGHTCPP_END

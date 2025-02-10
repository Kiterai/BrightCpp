#include "../interfaces/audio.hpp"
#include <brightcpp/common.hpp>
#include <vector>

BRIGHTCPP_START

namespace internal {

class audio_mixer {
    std::vector<audio_buffer_play_info> playing_list;
    audio_context_id id_serial_count = 0;

  public:
    audio_context_id add_playing(const audio_buffer_play_info &);
    void set_playing(audio_context_id id, const audio_buffer_play_info &);
    audio_buffer_play_info get_playing(audio_context_id id) const;
    void read(float *dst, size_t frames);
};

} // namespace internal

BRIGHTCPP_END

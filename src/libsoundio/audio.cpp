#define SOUNDIO_STATIC_LIBRARY
#include "audio.hpp"
#include <algorithm>
#include <iostream>
#include <optional>
#include <soundio/soundio.h>
#include <stdexcept>
#include <thread>
#include <vector>

BRIGHTCPP_START

namespace internal {

namespace libsoundio {

using write_sample_func = void(char *, float);
// using write_samples_func = void(SoundIoChannelArea *areas, int writable_frame_count, int channels_count, void* p);

static void write_sample_s16ne(char *ptr, float sample) {
    int16_t *buf = (int16_t *)ptr;
    float range = (float)INT16_MAX - (float)INT16_MIN;
    float val = sample * range / 2.0f;
    *buf = int16_t(val);
}

static void write_sample_s32ne(char *ptr, float sample) {
    int32_t *buf = (int32_t *)ptr;
    float range = (float)INT32_MAX - (float)INT32_MIN;
    float val = sample * range / 2.0f;
    *buf = int32_t(val);
}

static void write_sample_float32ne(char *ptr, float sample) {
    float *buf = (float *)ptr;
    *buf = sample;
}

static void write_sample_float64ne(char *ptr, float sample) {
    double *buf = (double *)ptr;
    *buf = sample;
}

static int default_buffer_length() {
#ifdef _WIN32
    return 1536;
#else
    return 480;
#endif
}

template <template <write_sample_func> class write_samples_callback>
class SoundIoOutStreamWrap {
    SoundIoOutStream *outstream;
    int buffer_length = default_buffer_length();
    void *write_samples_userdata;

    template <write_sample_func write_sample>
    static void write_callback(SoundIoOutStream *outstream, int frame_count_min, int frame_count_max) {
        auto const thiz = reinterpret_cast<SoundIoOutStreamWrap *>(outstream->userdata);

        float seconds_per_frame = 1.0f / outstream->sample_rate;

        SoundIoChannelArea *areas;

        if (frame_count_min > thiz->buffer_length || thiz->buffer_length > frame_count_max)
            thiz->buffer_length = std::clamp(thiz->buffer_length, frame_count_min, frame_count_max);
        const auto &buffer_length = thiz->buffer_length;

        int writing_frames_left = buffer_length;

        while (true) {
            int writable_frame_count = writing_frames_left;
            if (int err = soundio_outstream_begin_write(outstream, &areas, &writable_frame_count); err) {
                // std::cerr << "failed on soundio_outstream_begin_write: " << soundio_strerror(err) << std::endl;
                // TODO
                exit(-1);
            }

            if (writable_frame_count == 0)
                break;

            write_samples_callback<write_sample>{}(areas, writable_frame_count, outstream->layout.channel_count, thiz->write_samples_userdata);

            if (int err = soundio_outstream_end_write(outstream); err) {
                if (err == SoundIoErrorUnderflow)
                    return;
                // std::cerr << "failed on soundio_outstream_end_write: " << soundio_strerror(err) << std::endl;
                // TODO
                exit(-1);
            }

            writing_frames_left -= writable_frame_count;
            if (writing_frames_left <= 0)
                break;
        }
    }
    static void underflow_callback(SoundIoOutStream *outstream) {
        auto const thiz = reinterpret_cast<SoundIoOutStreamWrap *>(outstream->userdata);
        // std::cerr << "audio buffer underflow" << std::endl;
        // TODO
    }
    static void error_callback(SoundIoOutStream *outstream, int err) {
        auto const thiz = reinterpret_cast<SoundIoOutStreamWrap *>(outstream->userdata);
        // std::cerr << "outstream error: " << soundio_strerror(err) << std::endl;
        // TODO
    }

  public:
    SoundIoOutStreamWrap(SoundIoDevice *device, void *userdata)
        : outstream{soundio_outstream_create(device)},
          write_samples_userdata{userdata} {
        constexpr double latency = 0.01;

        outstream->userdata = this;
        outstream->underflow_callback = underflow_callback;
        outstream->software_latency = latency;
        outstream->error_callback = error_callback;

        if (soundio_device_supports_format(device, SoundIoFormatFloat32NE)) {
            outstream->format = SoundIoFormatFloat32NE;
            outstream->write_callback = write_callback<write_sample_float32ne>;
        } else if (soundio_device_supports_format(device, SoundIoFormatFloat64NE)) {
            outstream->format = SoundIoFormatFloat64NE;
            outstream->write_callback = write_callback<write_sample_float64ne>;
        } else if (soundio_device_supports_format(device, SoundIoFormatS32NE)) {
            outstream->format = SoundIoFormatS32NE;
            outstream->write_callback = write_callback<write_sample_s32ne>;
        } else if (soundio_device_supports_format(device, SoundIoFormatS16NE)) {
            outstream->format = SoundIoFormatS16NE;
            outstream->write_callback = write_callback<write_sample_s16ne>;
        } else {
            throw std::runtime_error("no suitable format available");
        }
    }
    SoundIoOutStreamWrap(const SoundIoOutStreamWrap &) = delete;
    SoundIoOutStreamWrap &operator=(const SoundIoOutStreamWrap &) = delete;
    SoundIoOutStreamWrap(SoundIoOutStreamWrap &&o) {
        buffer_length = o.buffer_length;
        write_samples_userdata = o.write_samples_userdata;
        outstream = o.outstream;
        o.outstream = nullptr;

        outstream->userdata = this;
    };
    SoundIoOutStreamWrap &operator=(SoundIoOutStreamWrap &&o) {
        buffer_length = o.buffer_length;
        write_samples_userdata = o.write_samples_userdata;
        outstream = o.outstream;
        o.outstream = nullptr;

        outstream->userdata = this;
        return *this;
    };
    ~SoundIoOutStreamWrap() {
        if (outstream)
            soundio_outstream_destroy(outstream);
    }

    void open() {
        if (int err = soundio_outstream_open(outstream); err)
            throw std::runtime_error("failed on soundio_outstream_open:" + std::string(soundio_strerror(err)));
        if (outstream->layout_error)
            throw std::runtime_error("unable to set channel layout:" + std::string(soundio_strerror(outstream->layout_error)));
    }
    void start() {
        if (int err = soundio_outstream_start(outstream); err)
            throw std::runtime_error("failed on soundio_outstream_start:" + std::string(soundio_strerror(err)));
    }
    void pause() {
        soundio_outstream_pause(outstream, true);
    }
    auto operator->() {
        return outstream;
    }
};

class SoundIoOutputDeviceWrap {
    SoundIoDevice *device;

  public:
    SoundIoOutputDeviceWrap(SoundIo *soundio, int index) : device{soundio_get_output_device(soundio, index)} {
        if (!device)
            throw std::runtime_error("out of memory");
        if (device->probe_error)
            throw std::runtime_error("cannot probe device:" + std::string(soundio_strerror(device->probe_error)));
    }
    SoundIoOutputDeviceWrap(const SoundIoOutputDeviceWrap &) = delete;
    SoundIoOutputDeviceWrap &operator=(const SoundIoOutputDeviceWrap &) = delete;
    SoundIoOutputDeviceWrap(SoundIoOutputDeviceWrap &&o) {
        device = o.device;
        o.device = nullptr;
    }
    SoundIoOutputDeviceWrap &operator=(SoundIoOutputDeviceWrap &&o) {
        device = o.device;
        o.device = nullptr;
        return *this;
    }
    ~SoundIoOutputDeviceWrap() {
        if (device)
            soundio_device_unref(device);
    }

    bool operator==(const SoundIoOutputDeviceWrap &o) {
        return soundio_device_equal(device, o.device);
    }
    bool operator!=(const SoundIoOutputDeviceWrap &o) {
        return !(*this == o);
    }

    template <template <write_sample_func> class write_samples_callback>
    SoundIoOutStreamWrap<write_samples_callback> create_outstream(void *userdata) {
        return SoundIoOutStreamWrap<write_samples_callback>(device, userdata);
    }
    auto operator->() {
        return device;
    }
};

class SoundIoWrap {
    SoundIo *soundio;
    bool device_changed = false;

  public:
    SoundIoWrap() : soundio{soundio_create()} {
        if (!soundio)
            throw std::runtime_error("failed to soundio_create()");
        if (int e = soundio_connect(soundio); e)
            throw std::runtime_error("failed to soundio_connect(): " + std::string(soundio_strerror(e)));
        soundio->userdata = this;

        soundio_force_device_scan(soundio);

        soundio->on_devices_change = [](SoundIo *soundio) {
            auto const thiz = reinterpret_cast<SoundIoWrap *>(soundio->userdata);
            thiz->device_changed = true;
        };

        flush_events();
    }
    ~SoundIoWrap() {
        soundio_destroy(soundio);
    }

    void flush_events() {
        soundio_flush_events(soundio);
    }
    std::vector<SoundIoOutputDeviceWrap> get_output_devices() {
        int device_count = soundio_output_device_count(soundio);
        std::vector<SoundIoOutputDeviceWrap> devices;
        for (int i = 0; i < device_count; i++)
            devices.emplace_back(soundio, i);
    }
    SoundIoOutputDeviceWrap get_default_output_device() {
        auto index = soundio_default_output_device_index(soundio);
        return SoundIoOutputDeviceWrap{soundio, index};
    }

    bool get_device_changed() {
        if (device_changed) {
            device_changed = false;
            return true;
        }
        return false;
    }
};

class audio_libsoundio : public audio_backend {
    std::vector<audio_buffer_play_info> playing_list;
    audio_context_id id_serial_count = 0;

    template <write_sample_func write_sample>
    struct write_samples {
        void operator()(SoundIoChannelArea *areas, int writable_frame_count, int channels_count, void *p) {
            auto thiz = reinterpret_cast<audio_libsoundio *>(p);

            for (int frame = 0; frame < writable_frame_count; frame++) {
                float sample = 0;
                for (auto &playing : thiz->playing_list) {
                    if (playing.paused)
                        continue;
                    sample += *playing.current_pos;
                    playing.current_pos++;
                    if (playing.current_pos >= playing.end_pos) {
                        switch (playing.mode) {
                        case audio_buffer_play_info::play_mode::normal:
                            playing.current_pos = playing.loop_pos;
                            playing.paused = true;
                            break;
                        case audio_buffer_play_info::play_mode::oneshot:
                            playing.stopped = playing.paused = true;
                            break;
                        case audio_buffer_play_info::play_mode::loop:
                            playing.current_pos = playing.loop_pos;
                            playing.end_pos = playing.next_loop_end_pos;
                            break;
                        case audio_buffer_play_info::play_mode::streaming_loop_invalid:
                            playing.paused = true;
                            break;
                        case audio_buffer_play_info::play_mode::streaming_loop_available:
                            playing.current_pos = playing.loop_pos;
                            playing.end_pos = playing.next_loop_end_pos;
                            playing.mode = audio_buffer_play_info::play_mode::streaming_loop_invalid;
                            // playing.callback(playing.callback_data, &playing);
                            break;
                        }
                    }
                }

                for (int channel = 0; channel < channels_count; channel++) {
                    write_sample(areas[channel].ptr, sample);
                    areas[channel].ptr += areas[channel].step;
                }
            }

            std::erase_if(thiz->playing_list, [](const auto &playing) {
                return playing.stopped;
            });
        }
    };

    std::optional<std::thread> audio_thread;
    bool running = true;

    SoundIoWrap soundio;
    SoundIoOutputDeviceWrap device;
    std::optional<SoundIoOutStreamWrap<write_samples>> outstream;

  public:
    audio_libsoundio()
        : soundio{},
          device{soundio.get_default_output_device()} {
        audio_thread = std::make_optional<std::thread>([this]() {
            outstream = device.create_outstream<write_samples>(this);
            outstream->open();
            outstream->start();
            while (running) {
                soundio.flush_events();
                if (soundio.get_device_changed() && device != soundio.get_default_output_device()) {
                    outstream->pause();
                    outstream.reset();

                    device = soundio.get_default_output_device();
                    outstream = device.create_outstream<write_samples>(this);
                    outstream->open();
                    outstream->start();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        });
    }
    ~audio_libsoundio() override {
        running = false;
        audio_thread->join();
    }

    int get_sample_rate() override {
        return (*outstream)->sample_rate;
    }

    audio_context_id play_audio_buffer(const audio_buffer_play_info &info) override {
        playing_list.push_back(info);
        playing_list.back().id = id_serial_count;
        id_serial_count++;
        return 0;
    }
    void set_playing_state(audio_context_id id, const audio_buffer_play_info &info) override {
        for (auto &playing : playing_list) {
            if (playing.id == id) {
                playing = info;
                playing.id = id;
                break;
            }
        }
    }
};

std::unique_ptr<audio_backend> make_libsoundio_manager() {
    return std::make_unique<audio_libsoundio>();
}

} // namespace libsoundio

} // namespace internal

BRIGHTCPP_END

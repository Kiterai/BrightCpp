#include <cstdint>

using write_sample_func = void(char *, float);

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
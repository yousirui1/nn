#ifndef __FEATURE_WINDOW_H__
#define __FEATURE_WINDOW_H__

#include "matrix.h"

#define M_2PI 6.283185307179586476925286766559005

struct frame_option_t
{
    float sample_rate;
    float frame_shift_ms;
    float frame_length_ms;
    float dither;
    float preemph_coeff;
    bool remove_dc_offset;
    char window_type[64];
    bool round_to_power_of_two;
    float blackman_coeff;
    bool snip_edges;
    bool allow_downsample;
    bool allow_upsample;
    int max_feature_vectors;
};

struct window_t
{
    struct frame_option_t frame_opt;
    matrix_t *frame;
    matrix_t *w;
};

int32_t window_shift_get(struct frame_option_t frame_opt);
int32_t window_size_get(struct frame_option_t frame_opt);
int32_t round_to_power_of_two_near(int32_t n);
int32_t padding_window_size(struct frame_option_t frame_opt);
int64_t first_sample_frame(int32_t frame, struct frame_option_t frame_opt);

int frame_option_init(struct frame_option_t *frame_opt);

struct window_t *window_init(struct frame_option_t frame_opt);
void window_deinit(struct window_t *window);

matrix_t * window_compute(struct window_t *window, int64_t sample_offset, matrix_t *wave, int32_t f,                    float *log_energy_pre_window);

int32_t window_num_frames(int64_t num_samples, struct frame_option_t frame_opt, bool flush);

#endif //  __FEATURE_WINDOW_H__

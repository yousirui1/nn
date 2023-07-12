#ifndef __FEATURE_WINDOW_H__
#define __FEATURE_WINDOW_H__


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

void frame_option_init(struct frame_option_t *frame_opt);


#endif //  __FEATURE_WINDOW_H__

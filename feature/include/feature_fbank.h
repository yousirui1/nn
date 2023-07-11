#ifndef __FEATURE_FBANK_H__
#define __FEATURE_FBANK_H__

#include <stdbool.h>

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

struct fbank_option_t
{
    struct frame_option_t frame_opt;
    //struct mel_option mel_opt
    bool use_energy;
    float energy_floor;
    bool raw_energy;
    bool htk_compat;
    bool use_log_fbank;
    bool use_power;
};


struct fbank_t
{
    float log_energy_floor;
    //matrix_t *mel_banks;

    struct fbank_option_t fbank_opt;
    //(void *)srfft;
    //struct 
};



#endif //  __FEATURE_FBANK_H__

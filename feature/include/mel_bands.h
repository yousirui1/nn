#ifndef __MEL_BANDS_H__
#define __MEL_BANDS_H__

#include "matrix.h"
#include "feature_window.h"

struct mel_bands_option_t
{
    int num_bins;
    float low_freq;
    float high_freq;
    float vtln_low;
    float vtln_high;

    bool htk_mode;
};

struct bin_t{
    int size;
    int offset;
    float *data;
};

struct mel_bands_t
{
    struct mel_bands_option_t mel_bands_opt;
    matrix_t *center_freqs;
    matrix_t *mel_energies;
    struct bin_t *bins;
};

int mel_bands_option_init(struct mel_bands_option_t *mel_bands_opt);
struct mel_bands_t *mel_bands_init(struct mel_bands_option_t mel_bands_opt,
                                       struct frame_option_t frame_opt, float vtln_warp_factor);
void mel_bands_deinit(struct mel_bands_t *mel_bands);
matrix_t *mel_bands_compute(struct mel_bands_t *mel_bands, matrix_t *power_spectrum);

#endif //  __MEL_BANDS_H__

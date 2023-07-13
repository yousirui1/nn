#ifndef __FEATURE_FBANK_H__
#define __FEATURE_FBANK_H__

#include "matrix.h"
#include "mel_bands.h"
#include "srfft.h"
#include "feature_window.h"

struct fbank_option_t
{
    struct frame_option_t frame_opt;
    struct mel_bands_option_t mel_bands_opt;
    bool use_energy;
    float energy_floor;
    bool raw_energy;
    bool htk_compat;
    bool use_log_fbank;
    bool use_power;
    int fft_size;
};


struct fbank_t
{
    float log_energy_floor;
    struct fbank_option_t fbank_opt;

    struct window_t *window;;
    struct mel_bands_t *mel_bands;
    struct rdft_t *rdft;
    matrix_t *feature;
    matrix_t *power_spectrum;
};


int fbank_option_init(struct fbank_option_t *fbank_opt);
struct fbank_t *fbank_init(struct fbank_option_t fbank_opt, float vtln_warp);
void fbank_deinit(struct fbank_t *fbank);
matrix_t* fbank_compute(struct fbank_t *fbank, matrix_t *wave);


#endif //  __FEATURE_FBANK_H__

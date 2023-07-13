#include "base.h"
#include "matrix.h"
#include "feature_fbank.h"

static void test_sample()
{
    LOG_DEBUG("==========TestSimple()=============");
    matrix_t *v = matrix_alloc(1, 1024);

    LOG_DEBUG("v->shape.size %d ", v->shape.size);
    for(int i = 0; i < v->shape.size; i++)
    {   
        v->data[i] = (abs( i * 433024253 ) % 65535) - (65535 / 2); 
    }   

    //matrix_print(v, "NULL");

    struct fbank_option_t fbank_opt;
    struct fbank_t *fbank = NULL;

    fbank_option_init(&fbank_opt);

    fbank_opt.frame_opt.dither = 0.0f;
    fbank_opt.frame_opt.preemph_coeff = 0.0f;
    strcpy(fbank_opt.frame_opt.window_type, "hanning");
    fbank_opt.frame_opt.remove_dc_offset = false;
    fbank_opt.frame_opt.round_to_power_of_two = true;
    fbank_opt.mel_bands_opt.low_freq = 0.0f;
    fbank_opt.htk_compat = true;
    fbank_opt.use_energy = true;;
        
    fbank = fbank_init(fbank_opt, 1.0);
   
    matrix_t *feature = fbank_compute(fbank, v);

    matrix_print(feature, "feature");
    return;
}

int main(int argc, char *argv[])
{
    test_sample();
}

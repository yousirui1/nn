#include "base.h"
#include "matrix.h"
#include "feature_fbank.h"

static struct mel_bands_t * mel_bands_get(float vtln)
{
    //rb 
    return NULL;
}

void fbank_option_init(struct fbank_option_t *fbank_opt)
{
    /* default options */
    if(fbank_opt)
    {
	    memset(fbank_opt, 0, sizeof(struct fbank_option_t));
        //mel_option_init(&(fbank_opt->mel_option));
	    //fbank_opt.mel_opt.low_freq = 0.0f;
	    fbank_opt->use_energy = false;
	    fbank_opt->energy_floor = 0.0f;
	    fbank_opt->raw_energy = true;
	    fbank_opt->htk_compat = false;
	    fbank_opt->use_log_fbank = false;
	    fbank_opt->use_power = true;
    }
}

struct fbank_t* fbank_init(struct fbank_option_t fbank_opt)
{
   struct fbank_t *fbank = (struct fbank_t *)malloc(sizeof(struct fbank_t));

   if(fbank_opt.energy_floor > 0.0)
       fbank->log_energy_floor = logf(fbank_opt.energy_floor);

   //if()
        ;
    //fbank->mel_bands = matrix_alloc();
    //mel_bands_get();
}

int compute_power_spectrum(matrix_t *mat)
{
    int32_t i;
    int32_t dim = mat->shape.size;
    int32_t half_dim = dim / 2;

    float first_energy = mat->data[0] * mat->data[0];
    float last_energy = mat->data[1] * mat->data[1];
    float real = 0.0f, im = 0.0f;

    /*
     * now we have in waveform, first half of complex spectrum
     * it's stored as [real0, realN/2, real1, im1, real2, im2, ...]
     */
    for(i = 1; i < half_dim; i++)
    {
        real = mat->data[i * 2];
        im = mat->data[i * 2 + 1];

        mat->data[i] = real * real + im * im;
    }
    mat->data[0] = first_energy;
    mat->data[half_dim] = last_energy;
    return SUCCESS;
}

int fbank_compute(struct fbank_t * fbank, float vtln_warp, matrix_t *signal_frame, matrix_t *feature)
{
    int32_t mel_offset = 0;
    float signal_raw_log_energy = 0.0f;
    struct mel_bands_t *mel_bands = mel_bands_get(vtln_warp);
    struct fbank_option_t fbank_opt = fbank->fbank_opt;
    matrix_t *power_spectrum = NULL;
    matrix_t *mel_energies = NULL;

    if(fbank_opt.use_energy && !fbank_opt.raw_energy)
        signal_raw_log_energy = matrix_log_energy(signal_frame);

#if 0
    if(fbank->srfft)
        fbank->srfft(signal_frame, true);
    else
        ;//realfft();
#endif

#if 0
    /* convert the fft into a power spectrum */
    compute_power_spectrum(signal_frame);

    power_spectrum = matrix_alloc(1, signal_frame->shape.size / 2 + 1);
    matrix_apply_copy(power_spectrum, 0, signal_frame, 0, signal_frame->shape.size / 2 + 1);
    if(!fbank_opt.use_power)
        matrix_apply_pow(power_spectrum, 0.5);

    mel_offset = ((fbank_opt.use_energy && !fbank_opt.htk_compat) ? 1 : 0);

    mel_banks_compute(mel_bands, power_spectrum, mel_energies);

    if(fbank_opt.use_log_fbank)
    {
        matrix_apply_floor(mel_energies);
        matrix_apply_log(mel_energies);
    }

    /* copy energy as first value (or the last, if htk_compat == true) */
    if(fbank_opt.use_energy)
    {

    }
#endif
}

static void test_sample()
{
    LOG_DEBUG("==========TestSimple()=============");
    matrix_t *v = matrix_alloc(1, 100000);
    matrix_t *feature = NULL;

    LOG_DEBUG("v->shape.size %d ", v->shape.size);
    for(int i = 0; i < v->shape.size; i++)
    {   
        v->data[i] = (abs( i * 433024253 ) % 65535) - (65535 / 2); 
    }   

	struct fbank_option_t fbank_opt;
    struct fbank_t *fbank = NULL;

    fbank_option_init(&fbank_opt);

    fbank_opt.frame_opt.dither = 0.0f;
    fbank_opt.frame_opt.preemph_coeff = 0.0f;
    strcpy(fbank_opt.frame_opt.window_type, "rectangular");
    fbank_opt.frame_opt.remove_dc_offset = false;
    fbank_opt.frame_opt.round_to_power_of_two = true;
    //fbank_opt.mel_opt.low_freq = 0.0f;
    fbank_opt.htk_compat = true;
    fbank_opt.use_energy = true;;
    
	fbank = fbank_init(fbank_opt);

    fbank_compute(fbank, 1.0, v, feature);

}


int main(int argc, char *argv[])
{
    test_sample();
}

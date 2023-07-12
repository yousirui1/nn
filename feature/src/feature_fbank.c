#include "base.h"
#include "matrix.h"
#include "mel_bands.h"
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
	    fbank_opt->use_energy = false;
	    fbank_opt->energy_floor = 0.0f;
	    fbank_opt->raw_energy = true;
	    fbank_opt->htk_compat = false;
	    fbank_opt->use_log_fbank = true;
	    fbank_opt->use_power = true;
    }
}

struct fbank_t* fbank_init(struct fbank_option_t fbank_opt)
{
   struct fbank_t *fbank = (struct fbank_t *)malloc(sizeof(struct fbank_t));

   if(fbank)
   {
        if(fbank_opt.energy_floor > 0.0)
            fbank->log_energy_floor = logf(fbank_opt.energy_floor);

        fbank->fbank_opt = fbank_opt;

   //if()
        ;
    //fbank->mel_bands = matrix_alloc();
    //mel_bands_get();
   }
    return fbank;
}

int need_raw_log_energy(struct fbank_option_t fbank_opt)
{
    return fbank_opt.use_energy && fbank_opt.raw_energy;
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
    struct mel_bands_t *mel_bands = fbank->mel_bands;
    struct fbank_option_t fbank_opt = fbank->fbank_opt;
    struct mel_bands_option_t mel_bands_opt = fbank_opt.mel_bands_opt;
    matrix_t *power_spectrum = NULL;
    matrix_t *mel_energies = NULL;

    if(fbank_opt.use_energy && !fbank_opt.raw_energy)
        signal_raw_log_energy = matrix_log_energy(signal_frame);

    if(fbank->rdft)
    {
        srfft(signal_frame, fbank->rdft,  true);
    }
    else
        ;//realfft();

    /* convert the fft into a power spectrum */
    compute_power_spectrum(signal_frame);


    power_spectrum = matrix_alloc(1, signal_frame->shape.size / 2 + 1);
    matrix_apply_copy(power_spectrum, 0, signal_frame, 0, signal_frame->shape.size / 2 + 1);

    matrix_print(power_spectrum, "power_spectrum");

    if(!fbank_opt.use_power)
        matrix_apply_pow(power_spectrum, 0.5);

    mel_offset = ((fbank_opt.use_energy && !fbank_opt.htk_compat) ? 1 : 0);
    mel_energies = mel_bands_compute(mel_bands, power_spectrum);

    if(fbank_opt.use_log_fbank)
    {
        LOG_DEBUG("use_log_fbank");
        matrix_apply_floor(mel_energies, FLT_EPSILON);
        matrix_apply_log(mel_energies);
    }

    /* copy energy as first value (or the last, if htk_compat == true) */
    if(fbank_opt.use_energy)
    {
        LOG_DEBUG("use_log_fbank");
        if(fbank_opt.energy_floor > 0.0 && signal_raw_log_energy < fbank->log_energy_floor)
        {
            signal_raw_log_energy = fbank->log_energy_floor;
        }
        int energy_index = fbank_opt.htk_compat ? mel_bands_opt.num_bins: 0;
        mel_energies->data[energy_index] = signal_raw_log_energy;
    }
    matrix_print(mel_energies, "feature");
}

matrix_t *fbank_compute_wave(struct fbank_t *fbank, matrix_t *wave, float vtln_warp)
{
    int i;
    int rows_out = 0, cols_out = 0;
    int use_raw_log_energy = 0;
    float raw_log_energy = 0.0f;
    struct mel_bands_t *mel_bands = mel_bands_get(vtln_warp);
    struct fbank_option_t fbank_opt = fbank->fbank_opt;
    struct mel_bands_option_t mel_bands_opt = fbank_opt.mel_bands_opt;
    struct frame_option_t frame_opt = fbank_opt.frame_opt;

    matrix_t *output = NULL;
    matrix_t *window = NULL;
    matrix_t *output_row = NULL;

    rows_out = window_num_frames(wave->shape.size, &fbank_opt, true);
    cols_out = mel_bands_opt.num_bins + (fbank_opt.use_energy ? 1 : 0);
    use_raw_log_energy = need_raw_log_energy(fbank_opt);

    if(0 == rows_out)
    {
        output = NULL;
        return ;
    }
    output = matrix_alloc(2, rows_out, cols_out);
    output_row = matrix_empty(2, 1, cols_out);

    for(i = 0; i < rows_out; i++)
    {
        window_compute(0, wave, i, frame_opt, window, use_raw_log_energy ? &raw_log_energy:NULL);
        output_row->data = &output->data[i * cols_out];

        fbank_compute(fbank, vtln_warp, window, output_row);
    }
    return output;
}

static void test_sample()
{
    LOG_DEBUG("==========TestSimple()=============");
    matrix_t *v = matrix_alloc(1, 512);
    matrix_t *feature = NULL;

    LOG_DEBUG("v->shape.size %d ", v->shape.size);
    for(int i = 0; i < v->shape.size; i++)
    {   
        v->data[i] = (abs( i * 433024253 ) % 65535) - (65535 / 2); 
    }   

    for(int i = 400; i < v->shape.size; i++)
    {
        v->data[i] = 0.0f;
    }

    matrix_print(v, "NULL");

	struct fbank_option_t fbank_opt;
    struct fbank_t *fbank = NULL;

    fbank_option_init(&fbank_opt);
    frame_option_init(&fbank_opt.frame_opt);
    mel_bands_option_init(&fbank_opt.mel_bands_opt);

    fbank_opt.frame_opt.dither = 0.0f;
    fbank_opt.frame_opt.preemph_coeff = 0.0f;
    strcpy(fbank_opt.frame_opt.window_type, "rectangular");
    fbank_opt.frame_opt.remove_dc_offset = false;
    fbank_opt.frame_opt.round_to_power_of_two = true;
    fbank_opt.mel_bands_opt.low_freq = 0.0f;
    fbank_opt.htk_compat = true;
    fbank_opt.use_energy = true;;
    
	fbank = fbank_init(fbank_opt);
    fbank->rdft = srfft_init(512);
    fbank->mel_bands = mel_bands_init(fbank_opt.mel_bands_opt, fbank_opt.frame_opt,  1.0);

    fbank_compute(fbank, 1.0, v, feature);
}


int main(int argc, char *argv[])
{
    test_sample();
}

#include "base.h"
#include "feature_fbank.h"

static int need_raw_log_energy(struct fbank_option_t fbank_opt)
{
    return fbank_opt.use_energy && fbank_opt.raw_energy;
}

/* default option */
int fbank_option_init(struct fbank_option_t *fbank_opt)
{
    if(NULL == fbank_opt)
    {   
        LOG_DEBUG("fbank_opt is NULL");
	    return ERROR;
    }

    memset(fbank_opt, 0, sizeof(struct fbank_option_t));
    fbank_opt->use_energy = false;
    fbank_opt->energy_floor = 0.0f;
    fbank_opt->raw_energy = true;
    fbank_opt->htk_compat = false;
    fbank_opt->use_log_fbank = true;
    fbank_opt->use_power = true;
    fbank_opt->fft_size = 512;

    frame_option_init(&fbank_opt->frame_opt);
    mel_bands_option_init(&fbank_opt->mel_bands_opt);

    return SUCCESS;
}

struct fbank_t *fbank_init(struct fbank_option_t fbank_opt, float vtln_warp)
{
	struct fbank_t *fbank = (struct fbank_t *)malloc(sizeof(struct fbank_t));
    if(NULL == fbank)
    {
        LOG_DEBUG("fbank malloc size %d error %s", sizeof(struct fbank_t), strerror(errno));
        return NULL;
    }

    memset(fbank, 0, sizeof(struct fbank_t));

    if(fbank_opt.energy_floor > 0.0)
        fbank->log_energy_floor = logf(fbank_opt.energy_floor);

    fbank->window = window_init(fbank_opt.frame_opt);
    fbank->mel_bands = mel_bands_init(fbank_opt.mel_bands_opt, fbank_opt.frame_opt, vtln_warp);

    fbank->fbank_opt = fbank_opt;
    fbank->fbank_opt.fft_size = padding_window_size(fbank_opt.frame_opt);
    fbank->rdft = srfft_init(fbank->fbank_opt.fft_size);
    return fbank;
}

void fbank_deinit(struct fbank_t *fbank)
{
    if(fbank)
    {
        if(fbank->window)
            window_deinit(fbank->window);

        if(fbank->rdft)
            srfft_deinit(fbank->rdft);
        
        if(fbank->mel_bands)
            mel_bands_deinit(fbank->mel_bands);

        if(fbank->feature)
            matrix_free(fbank->feature);

        if(fbank->power_spectrum)
            matrix_free(fbank->power_spectrum);

        fbank->window = NULL;
        fbank->rdft = NULL;
        fbank->mel_bands = NULL;
        fbank->feature = NULL;
        fbank->power_spectrum = NULL;

        free(fbank);
    }
}

static int fbank_frame_compute(struct fbank_t * fbank, matrix_t *frame, matrix_t *feature)
{
    int32_t mel_offset = 0;
    float signal_raw_log_energy = 0.0f;
    struct mel_bands_t *mel_bands = fbank->mel_bands;
    struct fbank_option_t fbank_opt = fbank->fbank_opt;
    struct mel_bands_option_t mel_bands_opt = fbank_opt.mel_bands_opt;
    matrix_t *power_spectrum = NULL;
    matrix_t *mel_energies = NULL;

    if(NULL == mel_bands)
    {
        LOG_DEBUG("mel_bands is NULL");
        return ERROR;
    }
    
    if(fbank_opt.use_energy && !fbank_opt.raw_energy)
        signal_raw_log_energy = matrix_log_energy(frame);


    if(fbank->rdft)
    {   
        memcpy(fbank->rdft->fft_data , frame->data, fbank_opt.fft_size * sizeof(float));
        srfft(fbank->rdft, true);
        memcpy(frame->data, fbank->rdft->input_comples_spec, fbank_opt.fft_size * sizeof(float));
    }
    else
	{
		LOG_DEBUG("rdft is NULL");
        return ERROR;
	}

    /* convert the fft into a power spectrum */
    matrix_power_spectrum(frame);


    if(NULL == fbank->power_spectrum)
    {
        power_spectrum = matrix_alloc(1, frame->shape.size / 2 + 1);
        fbank->power_spectrum = power_spectrum;
    }
    else
    {
        power_spectrum = fbank->power_spectrum;
    }

    matrix_apply_copy(power_spectrum, 0, frame, 0, frame->shape.size / 2 + 1);


    if(!fbank_opt.use_power)
        matrix_apply_pow(power_spectrum, 0.5);

    mel_offset = ((fbank_opt.use_energy && !fbank_opt.htk_compat) ? 1 : 0);




    mel_energies = mel_bands_compute(mel_bands, power_spectrum);

    //matrix_print(mel_energies, "mel_energies");

    if(fbank_opt.use_log_fbank)
    {
        matrix_apply_floor(mel_energies, FLT_EPSILON);
        //matrix_print(mel_energies, "mel_energies");
        matrix_apply_log(mel_energies);
    }

    /* copy energy as first value (or the last, if htk_compat == true) */
    if(fbank_opt.use_energy)
    {
        if(fbank_opt.energy_floor > 0.0 && signal_raw_log_energy < fbank->log_energy_floor)
        {
            signal_raw_log_energy = fbank->log_energy_floor;
        }
        int energy_index = fbank_opt.htk_compat ? mel_bands_opt.num_bins: 0;
        mel_energies->data[energy_index] = signal_raw_log_energy;
    }
    matrix_apply_copy(feature, 0, mel_energies, 0, mel_energies->shape.size);
    return SUCCESS;
}

matrix_t* fbank_compute(struct fbank_t *fbank, matrix_t *wave)
{
    int i;
    int rows_out = 0, cols_out = 0;
    int use_raw_log_energy = 0;
    float raw_log_energy = 0.0f;
    struct fbank_option_t fbank_opt = fbank->fbank_opt;
    struct mel_bands_option_t mel_bands_opt = fbank_opt.mel_bands_opt;
    struct frame_option_t frame_opt = fbank_opt.frame_opt;

    matrix_t *frame = NULL;
    matrix_t *output = NULL;
    matrix_t *output_row = NULL;

    rows_out = window_num_frames(wave->shape.size, frame_opt, true);
    cols_out = mel_bands_opt.num_bins + (fbank_opt.use_energy ? 1 : 0); 
    use_raw_log_energy = need_raw_log_energy(fbank_opt);

    LOG_DEBUG("rows_out %d cols_out %d", rows_out, cols_out);
    if(0 == rows_out)
    {   
        LOG_DEBUG("rows_out must not 0");
        return NULL;
    }   

    if(fbank->feature)
    {
        if(fbank->feature->shape.size != rows_out * cols_out)       
        {
            shape_t shape;
            shape.num_dims = 2;
            shape.dims[0] = rows_out;
            shape.dims[1] = cols_out;
            matrix_resize(fbank->feature, shape);
        }
    }
    else
    {
        fbank->feature = matrix_alloc(2, rows_out, cols_out);
    }

    output = fbank->feature;
    output_row = matrix_empty(2, 1, cols_out);
    for(i = 0; i < rows_out; i++)
    {   
        frame = window_compute(fbank->window, 0, wave, i, 
                                use_raw_log_energy ? &raw_log_energy : NULL);
        //LOG_DEBUG("-------------------------");
        //matrix_print(frame,"window_frame");
        output_row->data = &(output->data[i * cols_out]);
        fbank_frame_compute(fbank, frame, output_row);
    }   
    matrix_free(output_row);
    return fbank->feature;
}


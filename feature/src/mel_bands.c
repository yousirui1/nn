#include "base.h"
#include "mel_bands.h"

static inline float inverse_mel_scale(float mel_freq)
{   
    return 700.0f * (expf(mel_freq / 1127.0f) - 1.0f);
}

static inline float mel_scale(float freq)
{   
    return 1127.0f * logf(1.0f + freq / 700.0f);
}

static float vtln_warp_freq(float vtln_low_cutoff, float vtln_high_cutoff,
                        float low_freq, float high_freq, float vtln_warp_factor,
                        float freq)

{
    if(freq < low_freq || freq > high_freq)
        return freq;
    
    if(vtln_low_cutoff > low_freq)
    {   
        LOG_DEBUG("");
    }
    
    if(vtln_high_cutoff < high_freq)
    {   
        LOG_DEBUG("");
    }

    float one = 1.0;
    float l = vtln_low_cutoff * max(one, vtln_warp_factor);
    float h = vtln_high_cutoff * min(one, vtln_warp_factor);
    float scale = 1.0 / vtln_warp_factor;
    float Fl = scale * l;
    float Fh = scale * h;
    
    if(l > low_freq && h < high_freq)
    {
     
    }
    
    float scale_left = (Fl - low_freq) / (l - low_freq);
    float scale_right = (high_freq - Fh) / (high_freq - h);
    
    if(freq < l)
    {   
        return low_freq + scale_left * (freq - low_freq);
    }
    else if(freq < h)
    {   
        return scale * freq;
    }
    else
    {   
        return high_freq + scale_right * (freq - high_freq);
    }
}

static float vtln_warp_mel_freq(float vtln_low_cutoff, float vtln_high_cutoff,
                        float low_freq, float high_freq, float vtln_warp_factor,
                        float mel_freq)
{
    return mel_scale(vtln_warp_freq(vtln_low_cutoff, vtln_high_cutoff,
                                    low_freq, high_freq,
                                    vtln_warp_factor, inverse_mel_scale(mel_freq)));
}

int mel_bands_option_init(struct mel_bands_option_t *mel_bands_opt)
{
    if(NULL == mel_bands_opt)
    {
        LOG_DEBUG("mel_bands_opt is NULL");
        return ERROR;
    }

    memset(mel_bands_opt, 0, sizeof(struct mel_bands_option_t));

    mel_bands_opt->num_bins = 23;
    mel_bands_opt->low_freq = 20.0;
    mel_bands_opt->high_freq = 0.0;
    mel_bands_opt->vtln_low = 100.0;
    mel_bands_opt->vtln_high = -500.0;
    mel_bands_opt->htk_mode = false;
    return SUCCESS;
}

struct mel_bands_t *mel_bands_init(struct mel_bands_option_t mel_bands_opt,
                               struct frame_option_t frame_opt, float vtln_warp_factor)
{

    int bin, i;
    int num_bins = mel_bands_opt.num_bins;
    if(num_bins < 3)
    {
        LOG_DEBUG("Must have at least 3 mel bins");
        return NULL;
    }
    struct mel_bands_t *mel_bands = (struct mel_bands_t *)malloc(sizeof(struct mel_bands_t));
    if(NULL == mel_bands)
    {
        LOG_DEBUG("mel_bands malloc size %d error %s", sizeof(struct mel_bands_t), strerror(errno));
        return NULL;
    }

    float sample_freq = frame_opt.sample_rate;
    int window_length_padded = padding_window_size(frame_opt);

    int num_fft_bins = window_length_padded / 2;
    float nyquist = 0.5 * sample_freq;

    float low_freq = mel_bands_opt.low_freq;
    float high_freq;

    float fft_bin_width = sample_freq / window_length_padded;
    float mel_low_freq, mel_high_freq, mel_freq_delta, vtln_low, vtln_high;
	matrix_t *this_bin = NULL;

 	if(window_length_padded % 2 != 0)
    {
        LOG_DEBUG("window_length_padded must 2 ");	
        free(mel_bands);
        return NULL;
    }

    if(mel_bands_opt.high_freq > 0.0)
        high_freq = mel_bands_opt.high_freq;
    else
        high_freq = nyquist + mel_bands_opt.high_freq;

    if(low_freq <0.0 || low_freq >= nyquist ||
            high_freq <= 0.0 || high_freq > nyquist ||
            high_freq <= low_freq)
    {
        LOG_DEBUG("low_freq or high_freq is error");
        free(mel_bands);
        return NULL;
    }

    fft_bin_width = sample_freq / window_length_padded;
    mel_low_freq = mel_scale(low_freq);
    mel_high_freq = mel_scale(high_freq);

    mel_freq_delta = (mel_high_freq - mel_low_freq) / (num_bins + 1);

    vtln_low = mel_bands_opt.vtln_low;
    vtln_high = mel_bands_opt.vtln_high;

    LOG_DEBUG("low_freq %.8f high_freq %.8f ", low_freq, high_freq);

    LOG_DEBUG("mel_low_freq %.8f mel_high_freq %.8f mel_freq_delta %.8f ", mel_low_freq,mel_high_freq,mel_freq_delta);


    if(vtln_high < 0.0)
    {
        vtln_high += nyquist;
    }

    LOG_DEBUG("vtln_low %.8f vtln_high %.8f ", vtln_low, vtln_high);

    if(vtln_warp_factor != 1.0 &&
         (vtln_low < 0.0 || vtln_low <= low_freq
          || vtln_low >= high_freq
          || vtln_high <= 0.0 || vtln_high >= high_freq
          || vtln_high <= vtln_low))
    {
        LOG_DEBUG("vtln_warp_factor or vtln_low or vtln_high is error ");
        free(mel_bands);
        return NULL;
    }

   	mel_bands->bins = (struct bin_t *)malloc(sizeof(struct bin_t) * num_bins);
    mel_bands->center_freqs = matrix_alloc(1, num_bins);
    mel_bands->mel_energies = matrix_alloc(1, num_bins);
    this_bin = matrix_alloc(1, num_fft_bins);

    for(bin = 0; bin < num_bins; bin++)
    {
        float left_mel = mel_low_freq + bin * mel_freq_delta;
        float center_mel = mel_low_freq + (bin + 1) * mel_freq_delta;
        float right_mel = mel_low_freq + (bin + 2) * mel_freq_delta;

        if(vtln_warp_factor != 1.0)
        {
            left_mel = vtln_warp_mel_freq(vtln_low, vtln_high, low_freq, high_freq,
                                        vtln_warp_factor, left_mel);
            center_mel = vtln_warp_mel_freq(vtln_low, vtln_high, low_freq, high_freq,
                                        vtln_warp_factor, center_mel);
            right_mel = vtln_warp_mel_freq(vtln_low, vtln_high, low_freq, high_freq,
                                        vtln_warp_factor, right_mel);
        }

		mel_bands->center_freqs->data[bin] = inverse_mel_scale(center_mel);
        /* this_bin will be a vector of coefficients that is only
         * nonzero where this mel bin is active
         */
        int first_index = -1, last_index = -1;
        float weight;
        for(i = 0; i < num_fft_bins; i++)
        {
            float freq = (fft_bin_width * i);
            float mel = mel_scale(freq);

            if(mel > left_mel && mel < right_mel)
            {
                if(mel <= center_mel)
                    weight = (mel - left_mel) / (center_mel - left_mel);
                else
                    weight = (right_mel - mel) / (right_mel - center_mel);

                this_bin->data[i] = weight;

                if(first_index == -1)
                    first_index = i;
                last_index = i;
            }
        }
        int size = last_index + 1 - first_index;

        mel_bands->bins[bin].offset = first_index;
        mel_bands->bins[bin].size = size;
        mel_bands->bins[bin].data = (float *)malloc(sizeof(float) *size);

        memcpy(mel_bands->bins[bin].data, &this_bin->data[first_index], size * sizeof(float));

        //Replicate a bug in HTK, for testing purposes
        if(mel_bands_opt.htk_mode && bin == 0 && mel_low_freq != 0.0)
        {
            mel_bands->bins[bin].data[0] = 0.0;
        }
    }
    matrix_free(this_bin);
    mel_bands->mel_bands_opt = mel_bands_opt;
	return mel_bands;
}

void mel_bands_deinit(struct mel_bands_t *mel_bands)
{
    if(mel_bands)
    {
        if(mel_bands->center_freqs)
            matrix_free(mel_bands->center_freqs);

        if(mel_bands->mel_energies)
            matrix_free(mel_bands->mel_energies);

        if(mel_bands->bins)
        {
            int i;
            int num_bins = mel_bands->mel_bands_opt.num_bins;
            for(i = 0; i < num_bins; i++)
            {
                if(mel_bands->bins[i].data)
                    free(mel_bands->bins[i].data);

                mel_bands->bins[i].data = NULL;
            }
            free(mel_bands->bins);
            mel_bands->center_freqs = NULL;
            mel_bands->mel_energies = NULL;
            mel_bands->bins = NULL;

        }
        free(mel_bands);
        mel_bands = NULL;
    }
}

static float power_energy(float *a, float *b, int size)
{
    int i = 0;
    float value = 0.0f;
    for(i = 0; i < size; i++)
    {   
        value += a[i] * b[i];
    }   
    return value;
}

matrix_t *mel_bands_compute(struct mel_bands_t *mel_bands, matrix_t *power_spectrum)
{
    int i;
    int num_bins = mel_bands->mel_bands_opt.num_bins;
    int size, offset;
    float energy = 0.0f;

    for(i = 0; i < num_bins; i++)
    {
        offset = mel_bands->bins[i].offset;
        size = mel_bands->bins[i].size;

        //printf("size %d offset %d \n", size, offset);

        energy = power_energy(mel_bands->bins[i].data, &power_spectrum->data[offset], size);

        if(mel_bands->mel_bands_opt.htk_mode && energy < 1.0)
            energy = 1.0;
        mel_bands->mel_energies->data[i] = energy;
    }
    return mel_bands->mel_energies;
}



#include "base.h"
#include "matrix.h"
#include "feature_window.h"
#include "mel_bands.h"

void mel_bands_option_init(struct mel_bands_option_t *mel_bands_opt)
{
    if(mel_bands_opt)
    {
        memset(mel_bands_opt, 0, sizeof(struct mel_bands_option_t));

        mel_bands_opt->num_bins = 23;
        mel_bands_opt->low_freq = 20.0;
        mel_bands_opt->high_freq = 0.0;
        mel_bands_opt->vtln_low = 100.0;
        mel_bands_opt->vtln_high = -500.0;
        mel_bands_opt->htk_mode = false;
    }
}

static inline float inverse_mel_scale(float mel_freq)
{
    return 700.0f * (expf(mel_freq / 1127.0f) - 1.0f);
}

static inline float mel_scale(float freq)
{
    return 1127.0f * logf(1.0f + freq / 700.0f);
}

float vtln_warp_freq(float vtln_low_cutoff, float vtln_high_cutoff, 
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

float vtln_warp_mel_freq(float vtln_low_cutoff, float vtln_high_cutoff, 
                        float low_freq, float high_freq, float vtln_warp_factor, 
                        float mel_freq)
{
    return mel_scale(vtln_warp_freq(vtln_low_cutoff, vtln_high_cutoff,
                                    low_freq, high_freq,
                                    vtln_warp_factor, inverse_mel_scale(mel_freq)));
}


struct mel_bands_t * mel_bands_init(struct mel_bands_option_t mel_bands_opt, 
                                struct frame_option_t frame_opt, float vtln_warp_factor)
{
    int bin, i;
    int num_bins = mel_bands_opt.num_bins;
    if(num_bins < 3)
    {
        LOG_DEBUG("Must have at least 3 mel bins");
        return ;
    }
    struct mel_bands_t *mel_bands = (struct mel_bands_t *)malloc(sizeof(struct mel_bands_t));

    float sample_freq = frame_opt.sample_rate;
    int window_length_padded = padding_window_size(frame_opt);

    if(window_length_padded % 2 != 0)
    {
        LOG_DEBUG("");
        return ;
    }

    int num_fft_bins = window_length_padded / 2;
    float nyquist = 0.5 * sample_freq;

    float low_freq = mel_bands_opt.low_freq;
    float high_freq;

    if(mel_bands_opt.high_freq > 0.0)
        high_freq = mel_bands_opt.high_freq;
    else
        high_freq = nyquist + mel_bands_opt.high_freq;

    if(low_freq <0.0 || low_freq >= nyquist ||
            high_freq <= 0.0 || high_freq > nyquist ||
            high_freq <= low_freq)
    {
        LOG_DEBUG(""); 
        return ;
    }
    
    float fft_bin_width = sample_freq / window_length_padded;
    float mel_low_freq = mel_scale(low_freq);
    float mel_high_freq = mel_scale(high_freq);

    float mel_freq_delta = (mel_high_freq - mel_low_freq) / (num_bins + 1);

    float vtln_low = mel_bands_opt.vtln_low;
    float vtln_high = mel_bands_opt.vtln_high;

    if(vtln_high < 0.0)
    {
        vtln_high += nyquist;
    }

    if(vtln_warp_factor != 1.0 && 
         (vtln_low < 0.0 || vtln_low <= low_freq
          || vtln_low >= high_freq
          || vtln_high <= 0.0 || vtln_high >= high_freq
          || vtln_high <= vtln_low))
    {
        LOG_DEBUG("");
        return ;
    }

    //mel_bands->bins = matrix_alloc(3, num_bins, 2,  num_bins);
    mel_bands->bins = (struct bin_t *)malloc(sizeof(struct bin_t) * num_bins);
    mel_bands->center_freqs = matrix_alloc(1, num_bins);
    mel_bands->mel_energies = matrix_alloc(1, num_bins);
    matrix_t *this_bin = matrix_alloc(1, num_fft_bins);

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
        for(i = 0; i < num_fft_bins; i++)
        {
            float freq = (fft_bin_width * i);
            float mel = mel_scale(freq);

            if(mel > left_mel && mel < right_mel)
            {
                float weight;
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

matrix_t * mel_bands_compute(struct mel_bands_t *mel_bands, matrix_t *power_spectrum)
{
    int i;
    int num_bins = mel_bands->mel_bands_opt.num_bins;
    int size, offset;
    float energy = 0.0f;

    for(i = 0; i < num_bins; i++)
    {
        offset = mel_bands->bins[i].offset;
        size = mel_bands->bins[i].size;
        energy = power_energy(mel_bands->bins[i].data, &power_spectrum->data[offset], size);

        if(mel_bands->mel_bands_opt.htk_mode && energy < 1.0)
            energy = 1.0;
        mel_bands->mel_energies->data[i] = energy;
    }
    return mel_bands->mel_energies;
}



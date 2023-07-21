#include "base.h"
#include "matrix.h"
#include "feature_fbank.h"
#include "wav.h"


static inline float Int16SampleToFloat(int16_t data) {
    //constexpr float kMultiplier = 1.0f / (1 << 15);
    float kMultiplier = 1.0f / (1 << 15);
    return data * kMultiplier;
}

static inline int16_t floatSampleToInt16(float sample) {
    sample = fmaxf(sample, -1.0f);
    sample = fminf(sample, 1.0f);
    return (int16_t)(((sample + 1) / 2) * (INT16_MAX - INT16_MIN) + INT16_MIN);
}

void audio_int16_to_float(float *dst_data, int16_t *src_data, int data_size)
{
    int i;
    for(i = 0; i < data_size; i++)
    {
        dst_data[i] = Int16SampleToFloat(src_data[i]);
    }
}

static void test_sample()
{
    LOG_DEBUG("==========TestSimple()=============");

    int ret;
    const char *file_name = "/home/ysr/project/c/open_source/kaldi/src/feat/test_data/test.wav";
    WavFile* fp = NULL;
    fp = wav_open(file_name, WAV_OPEN_READ);

    wav_seek(fp, 0, SEEK_END);
    long int count = wav_tell(fp);
    printf("count %d \n", count);
    wav_seek(fp, 0, SEEK_SET);
    int16_t *wav_buf = (int16_t *)malloc(sizeof(int16_t) * count);
    float *wave = (float *)malloc(sizeof(float) * count);
    ret = wav_read(fp, wav_buf, count);  // frame count
    wav_close(fp);
    audio_int16_to_float(wave, wav_buf, count);

    matrix_t *v = matrix_alloc(1, count);
    //v->data = wave;

    LOG_DEBUG("v->shape.size %d ", v->shape.size);
    for(int i = 0; i < v->shape.size; i++)
    {   
        //v->data[i] = (abs( i * 433024253 ) % 65535) - (65535 / 2); 
        v->data[i] = wave[i];
    }   

    //matrix_print(v, "NULL");

    struct fbank_option_t fbank_opt;
    struct fbank_t *fbank = NULL;

    fbank_option_init(&fbank_opt);

    fbank_opt.frame_opt.dither = 0.0f;
    //fbank_opt.frame_opt.preemph_coeff = 0.97f;
    fbank_opt.frame_opt.sample_rate = 32000;
    strcpy(fbank_opt.frame_opt.window_type, "hanning");
    //strcpy(fbank_opt.frame_opt.window_type, "rectangular");
    //fbank_opt.frame_opt.remove_dc_offset = false;
    fbank_opt.frame_opt.round_to_power_of_two = true;
    fbank_opt.mel_bands_opt.low_freq = 20.0f;
    fbank_opt.htk_compat = true;
    fbank_opt.use_energy = false;
    fbank_opt.mel_bands_opt.num_bins = 128;
        
    fbank = fbank_init(fbank_opt, 1.0);
   
    matrix_t *feature = fbank_compute(fbank, v);

    matrix_print(feature, "feature");
    return;
}

int main(int argc, char *argv[])
{
    test_sample();
}

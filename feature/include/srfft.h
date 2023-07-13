#ifndef __SRFFT_H__
#define __SRFFT_H__

struct rdft_t
{
    int fft_size;
    int fft_l21;
    float *fft_data;
    size_t *ip;    
    float *w; 
    float *real;
    float *imag;
    float *input_comples_spec;
};

struct rdft_t *srfft_init(int fft_size);
void srfft_deinit(struct rdft_t *rdft);
int srfft(struct rdft_t *rdft,  bool forward);

#endif //  __SRFFT_H__

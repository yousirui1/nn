#ifndef __FFT_H__
#define __FFT_H__


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
int srfft(matrix_t *mat, struct rdft_t *rdft,  bool forward);


#endif //  __FFT_H__

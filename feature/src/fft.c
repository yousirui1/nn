#include "base.h"
#include "matrix.h"
#include "fft.h"

struct rdft_t *srfft_init(int fft_size)
{
    struct rdft_t *rdft = (struct rdft_t *)malloc(sizeof(struct rdft_t));
    int fft_l21 = fft_size / 2 + 1;
    if(rdft)
    {
        rdft->fft_l21 = fft_l21;
        rdft->fft_size = fft_size;
        rdft->fft_data = (float *)malloc(sizeof(float) * fft_size);
        rdft->ip = (float *)malloc(sizeof(float) * (fft_size >> 1));
        rdft->w = (float *)malloc(sizeof(float) * (fft_size >> 1));
        rdft->real = (float *)malloc(sizeof(float) * fft_l21);
        rdft->imag = (float *)malloc(sizeof(float) * fft_l21);
        rdft->input_comples_spec = (float *)malloc(sizeof(float) * fft_l21 * 2);
        LOG_DEBUG("fft_size >> 1 %d fft_l21 %d ", fft_size >> 1, fft_l21 * 2);
    }
    return rdft;
}


int srfft(matrix_t *mat, struct rdft_t *rdft,  bool forward)
{
    int i;
    memcpy(rdft->fft_data, mat->data, sizeof(float) * rdft->fft_size);

    WebRtc_rdft(rdft->fft_size, 1, rdft->fft_data, rdft->ip, rdft->w);

    //to do 
    rdft->real[0] = rdft->fft_data[0];
    //rdft->real[rdft->fft_l21 - 1] = rdft->fft_data[1];
    rdft->imag[0] = rdft->fft_data[1];
    //rdft->imag[rdft->fft_l21 - 1] = rdft->fft_data[0];;

    for(i = 1; i < rdft->fft_l21 - 1; i++)
    {
        rdft->real[i] = rdft->fft_data[2 * i];
        rdft->imag[i] = -rdft->fft_data[2 * i + 1];
    }

    for(i = 0; i < rdft->fft_l21; i++)
    {
        rdft->input_comples_spec[i * 2] = rdft->real[i];
        rdft->input_comples_spec[i * 2 + 1] = rdft->imag[i];
    }

    memcpy(mat->data, rdft->input_comples_spec, rdft->fft_size * sizeof(float));
    return 0;
}

void srfft1(float* real, float* imag, int size) {
    if (size < 2) {
        return;
    }   

    int i, j, k;
    int m, mmax, istep;
    float temp_real, temp_imag;
    float wr, wi, wtemp, theta;
    float tempr, tempi;

    // Bit-reversal
    j = 0;
    for (i = 0; i < size - 1; i++) {
        if (i < j) {
            // Swap real part
            temp_real = real[j];
            real[j] = real[i];
            real[i] = temp_real;
            // Swap imaginary part
            temp_imag = imag[j];
            imag[j] = imag[i];
            imag[i] = temp_imag;
        }
        k = size / 2;
        while (k <= j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }  
    // Cooley-Tukey decimation-in-time radix-2 FFT
    mmax = 1;
    while (mmax < size) {
        istep = mmax * 2;
        theta = 2.0 * M_PI / mmax;
        wtemp = sin(0.5 * theta);
        wr = 1.0;
        wi = 0.0;
        for (m = 0; m < mmax; m++) {
            for (i = m; i < size; i += istep) {
                j = i + mmax;
                tempr = wr * real[j] - wi * imag[j];
                tempi = wr * imag[j] + wi * real[j];
                real[j] = real[i] - tempr;
                imag[j] = imag[i] - tempi;
                real[i] += tempr;
                imag[i] += tempi;
            }
            wtemp = wr;
            wr = wr * wtemp - wi * wtemp;
            wi = wi * wtemp + wr * wtemp;
        }
        mmax = istep;
    }
}

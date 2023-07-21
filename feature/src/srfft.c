#include "base.h"
#include "fft4g.h"
#include "srfft.h"

struct rdft_t *srfft_init(int fft_size)
{
    struct rdft_t *rdft = (struct rdft_t *)malloc(sizeof(struct rdft_t));
    int fft_l21 = fft_size / 2 + 1;
    if(rdft)
    {
        rdft->fft_l21 = fft_l21;
        rdft->fft_size = fft_size;
        rdft->fft_data = (float *)malloc(sizeof(float) * fft_size);
        rdft->ip = (size_t *)malloc(sizeof(size_t) * (fft_size >> 1));
        rdft->w = (float *)malloc(sizeof(float) * (fft_size >> 1));
        rdft->real = (float *)malloc(sizeof(float) * fft_l21);
        rdft->imag = (float *)malloc(sizeof(float) * fft_l21);
        rdft->input_comples_spec = (float *)malloc(sizeof(float) * fft_l21 * 2);

        if(NULL == rdft->ip || NULL == rdft->w || NULL == rdft->real || NULL == rdft->imag ||
                NULL == rdft->input_comples_spec)
        {
            LOG_DEBUG("rdft malloc error  %s",strerror(errno));
            srfft_deinit(rdft);
            return NULL;
        }
    }
    return rdft;
}

void srfft_deinit(struct rdft_t *rdft)
{
    if(rdft)
    {
        if(rdft->fft_data)
            free(rdft->fft_data);

        if(rdft->ip)
            free(rdft->ip);

        if(rdft->w)
            free(rdft->w);

        if(rdft->real)
            free(rdft->real);

        if(rdft->imag)
            free(rdft->imag);

        if(rdft->input_comples_spec)
            free(rdft->input_comples_spec);

        rdft->fft_data = NULL;
        rdft->ip = NULL;
        rdft->w = NULL;
        rdft->real = NULL;
        rdft->imag = NULL;
        rdft->input_comples_spec = NULL;
        
        free(rdft);
        rdft = NULL;
    }
}

int srfft(struct rdft_t *rdft,  bool forward)
{
    int i;
    if(NULL == rdft)
    {
        LOG_DEBUG("rdft is NULL");
        return ERROR;
    }
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
    return SUCCESS;
}


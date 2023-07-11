#include "base.h"
#include "matrix.h"

int srfft(matrix_t *mat, bool forward)
{
#if 0
    int i = 0, j = 0; 
    if (pInst)
    {    
        //FTT计算幅度和相位

        WebRtc_rdft(pInst->fftSize, 1, pInst->fFFTData, pInst->ip, pInst->w);
        pInst->real[0] = pInst->fFFTData[0];
        pInst->real[pInst->fftL21 - 1] = pInst->fFFTData[1];
        pInst->imag[0] = 0; 
        pInst->imag[pInst->fftL21 - 1] = 0; 
        for (i = 1; i < pInst->fftL21 - 1; i++) 
        {    
            pInst->real[i] = pInst->fFFTData[2 * i];
            pInst->imag[i] = pInst->fFFTData[2 * i + 1];//虚部
        }    
        //进行函数拼接concatenate
        for (i = 0; i < pInst->fftL21; i++) 
        {    
            pInst->input_comples_spec[i * 2] = pInst->real[i];
            pInst->input_comples_spec[i * 2 + 1] = pInst->imag[i];
        }    
    }    

    return 0;
#endif
}

#ifndef __IM2COL_H__
#define __IM2COL_H__

#if 0
void im2col_cpu(float* data_im,
        int channels, int height, int width,
        int ksize, int stride, int pad, float* data_col);
#endif

void im2col_cpu(float* data_im,
     int channels,  int height,  int width,
     int ksize_h, int ksize_w,  int stride_h, int stride_w, int pad, float* data_col);

#endif //  __IM2COL_H__

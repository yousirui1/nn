#ifndef __COL2IM_H__
#define __COL2IM_H__

void col2im_cpu(float* data_col,
         int channels,  int height,  int width,
         int ksize_h, int ksize_w,  int stride_h, int stride_w, int pad, float* data_im);


#endif //  __COL2IM_H__

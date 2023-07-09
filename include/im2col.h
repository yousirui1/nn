#ifndef __IM2COL_H__
#define __IM2COL_H__

#include "matrix.h"


void im2col_cpu(matrix_t *data,
     int ksize_h, int ksize_w,  int stride_h, int stride_w, int pad,
     matrix_t *data_col);

#endif //  __IM2COL_H__

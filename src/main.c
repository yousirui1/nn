#include "base.h"
#include "matrix.h"

#if 0
//划窗区域转换成2 列矩阵
void im2col_cpu(float *data_im, int channels, int height, int width,
        int ksize, int stride, int pad, float *data_col)
{
    int c, h, w;

    int height_col = (height + 2 * pad - ksize) / stride + 1;
    int width_col = (width + 2 * pad - ksize) / stride + 1;
    int channel_col = channel * ksize * ksize;
    //划窗列矩阵 （height_col * width_col, channel_col）

#if 0
    for(c = 0; c < channel_col; c++)
    {
        for(h = 0; h < height_col; h++)
        {
            for(w = 0; w < width_col; w ++)
            {

                data_col[c][h][w] = data_im()
            }
        }
    }
#endif
    for(h = 0; h < height_col; h++)
    {
        for(w = 0; w < width_col; w++)
        {
            int im_row = h * stride;
            int im_col = w * stride;

            data_col[h][w] = 
        }
    }
}

//#define N 0
#define C 2
#define H 0
#define W 1

int test(matrix_t *data, int ksize, int stride, int pad, matrix_t *data_col)
{
    int c, h, w;
    int channel_col = data_col->shape.dims[C];
    int height_col = data_col->shape.dims[H];
    int width_col = data_col->shape.dims[W];
    int im_col, im_row, c_im, h_offset, w_offset;
    int height = data->shape.dims[H];
    int width = data->shape.dims[W];
    int channel = data->shape.dims[C];
    int num_dims = data->shape.num_dims;
#if 0
    for(c = 0; c < channel_col; c++)
    {
        int h_offset = (c / ksize) % ksize;
        int w_offset = c % ksize;
        int c_im = c / ksize / ksize;
        for(h = 0;  h < height_col; h++)
        {
            for(w = 0; w < width_col; w++)
            {
                int im_row = h_offset + h * stride - pad;  //h起始位置
                int im_col = w_offset + w * stride - pad;  //w起始位置
                
               // if(im_row <  0 || im_col < 0 || im_row >= height || im_col >= weight)  //超过data index 都是pad,直接=0
                //    LOG_DEBUG("0 ");
                //else
                    LOG_DEBUG("[%d %d]", im_row, im_col);
            }
        }
    }
#endif
    for(h = 0;  h < height_col; h++)
    {
        for(w = 0; w < width_col; w++)
        {
            for(c = 0; c < channel_col; c++)
            {
                h_offset = (c / ksize) % ksize;
                w_offset = c % ksize;
                c_im = c / ksize / ksize;

                im_row = h_offset + h * stride - pad;  //h起始位置
                im_col = w_offset + w * stride - pad;  //w起始位置
                
                if(im_row <  0 || im_col < 0 || im_row >= height || im_col >= width)  //超过data index 都是pad,直接=0
                    matrix_set(data_col, 0.0f, num_dims, h, w, c);
                else
                    matrix_set(data_col, matrix_get(data, num_dims, im_row, im_col, c_im),  num_dims, h, w, c);
            }
        }
    }
}



int main(int argc, char *argv[])
{
    float input[25];
    float filter[] = {
        1, 0, -1,
        1, 0, -1,
        1, 0, -1,
    };

    //h, w, c
    //c, h, w


    matrix_t *mat = matrix_empty(3, 5, 5, 1);
    mat->data = input;
    matrix_t *weights = matrix_empty(3, 1, 3, 3);
    weights->data = filter;

    matrix_print(weights);

    for(int i = 0; i < 25; i++)
    {
        input[i] = 0.1 * i;
    }
    matrix_print(mat);

    int height = 5;
    int width = 5;
    int channel = 1;
    int pad = 0;
    int ksize = 3;
    int stride = 1;

    int height_col = (height + 2*pad - ksize) / stride + 1;
    int weight_col = (height + 2*pad - ksize) / stride + 1;
    int channel_col = channel * ksize * ksize; 

    matrix_t *col = matrix_alloc(3, channel_col,  height_col, weight_col);

    matrix_print(col);

    //shape_print(col->shape);
    //展开

    for(int n = 0; n < 1; n++)
    {
        test(mat, ksize, stride, pad, col);
        gemm(0, 0, m, n, k, 1, a, k, b, n, 1, c, n);
        //
    }
}
#endif

#ifndef __CONV_TRANSPOSE_LAYER_H__
#define __CONV_TRANSPOSE_LAYER_H__

#include "layer.h"
#include "matrix.h"

struct conv_transpose_weight_t
{
    struct matrix_t *weights;
    struct matrix_t *bias;
};

struct conv_transpose_layer_t
{
    shape_t output_shape;
    int filters;
    int out_channels;
    int kernel_h;
    int kernel_w;
    int stride_h;
    int stride_w;
    int padding;
    int output_padding;

    matrix_t *temp_data;
    matrix_t *output_data;

    matrix_t *weights;
    matrix_t *bias;
};

struct layer_t *conv_transpose_layer_alloc(void* filters, void *kernel_h, 
                    void* kernel_w, void *stride_h,void *stride_w, void *padding, 
                    void *output_padding);
void conv_transpose_layer_free(struct layer_t *layer);
int conv_transpose_load_weight(struct layer_t *layer, void *weight);
int conv_transpose_save_weight(struct layer_t *layer, void *weight);
struct matrix_t *conv_transpose_layer_forward(struct layer_t *layer, struct matrix_t *input_data);struct matrix_t *conv_transpose_layer_backward(struct layer_t *layer, struct matrix_t *input_data);

#endif //  __CONV_TRANSPOSE_LAYER_H__

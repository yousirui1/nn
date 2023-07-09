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
    struct shape_t input_shape;
    struct shape_t col;    
    int out_channels;
    int kernel_size;
    int stride;
    int padding;
    int output_padding;

    int input_size;
    int output_size;
    int col_size;

    struct matrix_t *input_col;
    struct matrix_t *output_data;

    struct matrix_t *weights;
    struct matrix_t *bias;
};

//struct layer_t *conv_transpose_layer_alloc();
void conv_transpose_layer_free(struct layer_t *layer);
int conv_transpose_load_weight(struct layer_t *layer, void *weight);
int conv_transpose_save_weight(struct layer_t *layer, void *weight);
//struct matrix_t *conv_transpose_layer_forward(struct layer_t *layer, struct matrix_t *input_data);
//struct matrix_t *conv_transpose_layer_backward(struct layer_t *layer, struct matrix_t *input_data);


#endif //  __CONV_TRANSPOSE_LAYER_H__

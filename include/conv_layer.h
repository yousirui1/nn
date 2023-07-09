#ifndef __CONV_LAYER_H__
#define __CONV_LAYER_H__

#include "layer.h"
#include "matrix.h"

struct conv_weight_t
{
    struct matrix_t *weights;
    struct matrix_t *bias;
};

struct conv_layer_t
{
    struct shape_t output_shape;

    int out_channels;
    int kernel_w;
    int kernel_h;
    int stride_w;
    int stride_h;
    int padding;

    struct matrix_t *output_data;

    struct matrix_t *weights;
    struct matrix_t *bias;
};

struct layer_t * conv_layer_alloc(void* out_channels, void *kernel_height, void* kernel_width, void *stride_height,                                     void *stride_width,  void *padding);
void conv_layer_free(struct layer_t *layer);
int conv_load_weight(struct layer_t *layer, void *weight);
int conv_save_weight(struct layer_t *layer, void *weight);
struct matrix_t *conv_layer_forward(struct layer_t *layer, struct matrix_t *input_data);
struct matrix_t *conv_layer_backward(struct layer_t *layer, struct matrix_t *input_data);


#endif //  __CONV_LAYER_H__

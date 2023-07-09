#ifndef __FC_LAYER_H__
#define __FC_LAYER_H__

#include "layer.h"
#include "matrix.h"

struct linear_weight_t
{
    struct matrix_t *weights;
    struct matrix_t *bias;
};

struct linear_layer_t
{
    int in_dim;
    int out_dim;

    struct matrix_t *weights;
    struct matrix_t *bias;
};

struct layer_t *linear_layer_alloc();
void linear_layer_free(struct layer_t *layer);
int linear_load_weight(struct layer_t *layer, void *weight);
int linear_save_weight(struct layer_t *layer, void *weight);
struct matrix_t *linear_layer_forward(struct layer_t *layer, struct matrix_t *input_data);
struct matrix_t *linear_layer_backward(struct layer_t *layer, struct matrix_t *input_data);


#endif //  __LINEAR_LAYER_H__

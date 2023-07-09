#ifndef __BN_LAYER_H__
#define __BN_LAYER_H__

#include "layer.h"
#include "matrix.h"

struct bn_weight_t
{
    matrix_t *weights;
    matrix_t *bias;

    matrix_t *mean;
    matrix_t *variance;
};

struct bn_layer_t
{
    int num_channels;
    int is_train;

    matrix_t *output_data;

    matrix_t *weights;
    matrix_t *bias;

    matrix_t *mean;
    matrix_t *variance;
};

struct layer_t *bn_layer_alloc(void* is_train);
void bn_layer_free(struct layer_t *layer);
int bn_load_weight(struct layer_t *layer, void *weight);
int bn_save_weight(struct layer_t *layer, void *weight);
struct matrix_t *bn_layer_forward(struct layer_t *layer, struct matrix_t *input_data);
struct matrix_t *bn_layer_backward(struct layer_t *layer, struct matrix_t *input_data);



#endif //  __BN_LAYER_H__

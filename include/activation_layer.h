#ifndef __ACTIVATION_LAYER_H__
#define __ACTIVATION_LAYER_H__

#include "layer.h"
#include "matrix.h"

enum activation
{
    RELU = 0,
    LEAKY_RELU,
    PRELU,
    TANH,
    SIGMOID,
    SOFTMAX
};


struct actiation_weight_t
{
    matrix_t *weights;
};

struct activation_layer_t
{
    int type;
    int num_channels;
    int padding_h;
    int padding_w;
    int padding_c;
    int offset;         //todo
    float (*activate)(float x, float alpha);
    matrix_t *output_data;
    matrix_t *pad_output_data;
    matrix_t *weights;
};

struct layer_t * activation_layer_alloc(void *_activation_name, void *padding_h, void *padding_w, void *offset);
void activation_layer_free(struct layer_t *layer);
int activation_load_weight(struct layer_t *layer, void *weight);
int activation_save_weight(struct layer_t *layer, void *weight);
struct matrix_t *activation_layer_forward(struct layer_t *layer, struct matrix_t *input_data);
struct matrix_t *activation_layer_backward(struct layer_t *layer, struct matrix_t *input_data);

float sigmoid(float x);

#endif //  __ACTIVATION_LAYER_H__

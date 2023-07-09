#ifndef __LAYER_H__
#define __LAYER_H__

#include "list.h"
#include "matrix.h"

#define MAX_DIMS 8


struct layer_t
{
    void *handle;
    void *param;
    char layer_name[128];
    shape_t input_shape;
    shape_t output_shape;

    struct list_head link;

    void (*alloc)(); // to do 
    struct matrix_t* (*forward)(struct layer_t *layer, struct matrix_t *input_data);
    struct matrix_t* (*backward)(struct layer_t *layer, struct matrix_t *input_data);
    void (*free)(struct layer_t *layer);
    int (*load_weight)(struct layer_t *layer, void *weight);
    int (*save_weight)(struct layer_t *layer, void *weight);
};
struct layer_t layer_t;

enum nn_layer{
    INPUT_LAYER = 0,
    LINEAR_LAYER,
    BN_LAYER,
    CONV_LAYER,
    CONV_TRANSPOSE_LAYER,
    LSTM_LAYER,
    ACTIVATION_LAYER,
};


struct support_layer_t
{
    enum nn_layer id; 
    const char *layer_name;
};




#endif //  __LAYER_H__

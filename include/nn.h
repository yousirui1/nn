#ifndef __NN_H__
#define __NN_H__

#include "list.h"
#include "matrix.h"

enum nn_mode{
    NN_TRAIN = 0,
    NN_EAVL,
};

struct nn_t
{
    char nn_name[128];
    int batch_size;
    int layer_size;
    struct list_head layer_root;
};

typedef struct nn_t nn_t;

void nn_weight_save();
int nn_load_layer_weight();
void nn_load_weight(struct nn_t *nn);
float nn_loss();
float nn_accurracy();
struct matrix_t * nn_forward(struct nn_t *nn, struct matrix_t *input_data);
void nn_backward();


#endif //  __NN_H__

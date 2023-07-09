#ifndef __LSTM_LAYER_H__
#define __LSTM_LAYER_H__

#include "layer.h"
#include "matrix.h"

struct lstm_weight_t
{
    matrix_t *prev_hidden_state;
    matrix_t *prev_cell_state;

    matrix_t *hidden_state;
    matrix_t *cell_state;
    
    matrix_t *weight_ih;
    matrix_t *weight_hh;

    matrix_t *bias_ih;
    matrix_t *bias_hh;
};

struct lstm_layer_t
{
    int input_size;
    int hidden_size;
    int num_weights;

    matrix_t *prev_hidden_state;
    matrix_t *prev_cell_state;

    matrix_t *hidden_state;
    matrix_t *cell_state;
    
    matrix_t *weight_ih;
    matrix_t *weight_hh;

    matrix_t *bias_ih;
    matrix_t *bias_hh;
};

struct layer_t *lstm_layer_alloc();
void lstm_layer_free(struct layer_t *layer);
int lstm_load_weight(struct layer_t *layer, void *weight);
int lstm_save_weight(struct layer_t *layer, void *weight);
struct matrix_t *lstm_layer_forward(struct layer_t *layer, struct matrix_t *input_data);
struct matrix_t *lstm_layer_backward(struct layer_t *layer, struct matrix_t *input_data);

#endif //  __LSTM_LAYER_H__

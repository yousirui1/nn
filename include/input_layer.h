#ifndef __INPUT_LAYER_H__
#define __INPUT_LAYER_H__

#include "layer.h"


struct input_layer_t
{
    struct shape_t output_shape;
};

struct layer_t *input_layer_alloc(void *input_shape, void *output_shape);
// input reshap  
struct matrix_t *input_layer_forward(struct layer_t *layer, struct matrix_t *input_data);
struct matrix_t *input_layer_forward(struct layer_t *layer, struct matrix_t *input_data);
void input_layer_free(struct layer_t *layer);
int input_load_weight(struct layer_t *layer);
int input_save_weight(struct layer_t *layer);



#endif //  __INPUT_LAYER_H__

#include "base.h"
#include "matrix.h"
#include "input_layer.h"

struct layer_t *input_layer_alloc(void *input_shape, void *output_shape)
{
    struct layer_t *layer = NULL;
    struct input_layer_t *input_layer = NULL;

    layer = (struct layer_t *)malloc(sizeof(struct layer_t));
    if(NULL == layer)
    {
        LOG_DEBUG("input layer malloc size %ld  error %s", sizeof(struct layer_t), strerror(errno));
        return NULL;
    }

    input_layer = (struct input_layer_t *)malloc(sizeof(struct input_layer_t));
    if(NULL == input_layer)
    {
        free(layer);
        LOG_DEBUG("input_layer handle malloc size %ld  error %s", sizeof(struct input_layer_t), strerror(errno));
        return NULL;
    }

    if(output_shape)
    {
        input_layer->output_shape = *(struct shape_t *)output_shape;
    }

    layer->handle = input_layer;
    return layer;
}

void input_layer_free(struct layer_t *layer)
{
    struct input_layer_t *input_layer = (struct input_layer *)layer->handle;

    if(input_layer)
    {
        free(input_layer);
    }
    input_layer = NULL;
    layer->handle = NULL;
}

struct matrix_t *input_layer_forward(struct layer_t *layer, struct matrix_t *input_data)
{
    struct input_layer_t *input_layer = (struct input_layer *)layer->handle;
    shape_print(input_data->shape);
    
    //to do reshape 
    //or feature 
    return input_data;
}

struct matrix_t *input_layer_backward(struct layer_t *layer, struct matrix_t *input_data)
{

}

int input_load_weight(struct layer_t *layer)
{

}

int input_save_weight(struct layer_t *layer)
{

}


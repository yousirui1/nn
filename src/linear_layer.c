#include "base.h"
#include "linear_layer.h"

struct layer_t * linear_layer_alloc(void *input_size, void *output_size)
{
    struct layer_t *layer = NULL;
    struct linear_layer_t *linear_layer = NULL;

    layer = (struct layer_t *)malloc(sizeof(struct layer_t));
    if(NULL == layer)
    {   
        LOG_DEBUG("linear layer malloc size %ld  error %s", sizeof(struct layer_t), strerror(errno));
        return NULL;
    }   

    linear_layer = (struct linear_layer_t *)malloc(sizeof(struct linear_layer_t));
    if(NULL == linear_layer)
    {   
        free(layer);
        LOG_DEBUG("linear_layer handle malloc size %ld  error %s", sizeof(struct linear_layer_t), strerror(errno));
        return NULL;
    }     

	linear_layer->input_size = *(int *)input_size;
	linear_layer->output_size = *(int *)output_size;

    //linear_layer->weights = matrix_alloc(linear_layer->in_dim, linear_layer->out_dim);

    //linear_layer->bias = matrix_alloc(1, linear_layer->out_dim);

    layer->handle = linear_layer;

    return layer;
}

void linear_layer_free(struct layer_t *layer)
{
    struct linear_layer_t *linear_layer = (struct linear_layer_t *)layer->handle;

    if(linear_layer)
    {
        free(linear_layer);
    }
    linear_layer = NULL;
    layer->handle = NULL;
}

int linear_load_weight(struct layer_t *layer, void *weight)
{
    struct linear_layer_t *linear_layer = (struct linear_layer_t *)layer->handle;
    struct linear_weight_t *linear_weight = (struct linear_weight_t *)weight;

    if(linear_weight)
    {
        if(linear_weight->weights && linear_weight->bias)
        {
            if(linear_layer->weights)
            {
                matrix_free(linear_layer->weights);
            }

            linear_layer->weights = matrix_copy(linear_weight->weights);
            if(linear_layer->bias)
            {
                matrix_free(linear_layer->bias);
            }
            linear_layer->bias = matrix_copy(linear_weight->bias);
            return SUCCESS;
        }
    }
    return ERROR;
}

int linear_save_weight(struct layer_t *layer, void *weight)
{

}

struct matrix_t *linear_layer_forward(struct layer_t *layer, struct matrix_t *input_data)
{
#if 0
	struct matrix_t *output_data = NULL;
    struct linear_layer_t *linear_layer = (struct linear_layer_t *)layer->handle;
    output_data = matrix_multiply(input_data, linear_layer->weights);
    matrix_apply_sum(output_data, linear_layer->bias, 1);
    return output_data;
#endif
}

struct matrix_t *linear_layer_backward(struct layer_t *layer, struct matrix_t *input_data)
{

}

#if 0
int main(int argc, char *argv[])
{
    int input_size = 10;
    int output_size = 5;

    struct layer_t *layer = linear_layer_alloc(&input_size, &output_size);
    struct linear_layer_t *linear_layer = NULL;
    linear_layer = (struct linear_layer_t *)layer->handle;

    struct linear_weight_t linear_weight;
    struct matrix_t *output_data = NULL;
    struct matrix_t *input_data = NULL;

    float weights[] = {
         0.2917, -0.0472, -0.1961, -0.1801,  0.1901, -0.0147, -0.2585, -0.1090,
         -0.0168,  0.3039,
        -0.0047,  0.1831, -0.2080,  0.2487,  0.2681,  0.3005, -0.2075,  0.0076,
         -0.0745, -0.0621,
         0.2936, -0.0320, -0.2501, -0.2320,  0.2683, -0.1857, -0.2480, -0.1078,
         -0.2276,  0.2115,
        -0.1754, -0.0677,  0.2851, -0.1203, -0.1737, -0.1284,  0.2748,  0.2149,
         -0.0095,  0.0226,
         0.0319, -0.0401, -0.1988,  0.2547, -0.2965, -0.1517,  0.2780,  0.1163,
         -0.2123,  0.1824
		};


    float bias[] = {
         //   -1.2593,  0.6562,  1.0147, -0.1668, -0.2626
            0.0, 0.0, 0.0, 0.0, 0.0
    };

    float input[] = {
        -1.1278,  0.7362, -1.0249,  1.8824, -0.2773, -0.5243, -0.1489, -1.9385,
                 -1.5971, -0.4499
    };

    linear_weight.weights = matrix_alloc_empty(10, 5);
    linear_weight.bias = matrix_alloc_empty(1, 5);

    linear_weight.weights->data = weights;
    linear_weight.bias->data = bias;

    linear_load_weight(layer, &linear_weight);
    
    input_data = matrix_alloc_empty(1, 10);
    input_data->data = input;

    output_data = linear_layer_forward(layer, input_data);

    for(int i = 0; i < output_data->rows * output_data->cols; i++)
    {
        printf("%f ", output_data->data[i]);
    }
    printf("\n");
}
#endif

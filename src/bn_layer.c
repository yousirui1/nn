#include "base.h"
#include "bn_layer.h"

struct layer_t * bn_layer_alloc(void* is_train)
{
    struct layer_t *layer = NULL;
    struct bn_layer_t *bn_layer = NULL;

    layer = (struct layer_t *)malloc(sizeof(struct layer_t));
    if(NULL == layer)
    {   
        LOG_DEBUG("bn layer malloc size %ld  error %s", sizeof(struct layer_t), strerror(errno));
        return NULL;
    }   

    bn_layer = (struct bn_layer_t *)malloc(sizeof(struct bn_layer_t));
    if(NULL == bn_layer)
    {   
        free(layer);
        LOG_DEBUG("bn_layer handle malloc size %ld  error %s", sizeof(struct bn_layer_t), strerror(errno));
        return NULL;
    }     

    bn_layer->is_train = *(int *)is_train;
    layer->handle = bn_layer;
    return layer;
}

void bn_layer_free(struct layer_t *layer)
{
    struct bn_layer_t *bn_layer = (struct bn_layer_t *)layer->handle;
    if(layer)
    {
        if(bn_layer)
        {
            free(bn_layer);
        }

        bn_layer = NULL;
        layer->handle = NULL;
        free(layer);
    }
}

int bn_load_weight(struct layer_t *layer, void *weight)
{
    struct bn_layer_t *bn_layer = (struct bn_layer_t *)layer->handle;
    struct bn_weight_t *bn_weight = (struct bn_weight_t *)weight;
    if(bn_weight)
    {   
        if(bn_layer->weights)
        {
            matrix_free(bn_layer->weights);
        }
        bn_layer->weights = matrix_copy(bn_weight->weights);

        if(bn_layer->bias)
        {
            matrix_free(bn_layer->bias);
        }
        bn_layer->bias = matrix_copy(bn_weight->bias);

        if(bn_layer->variance)
        {
            matrix_free(bn_layer->variance);
        }
        bn_layer->variance = matrix_copy(bn_weight->variance);

        if(bn_layer->mean)
        {
            matrix_free(bn_layer->mean);
        }
        bn_layer->mean = matrix_copy(bn_weight->mean);

        bn_layer->num_channels = bn_layer->weights->shape.dims[C];  
    }
    return ERROR;
}

int bn_save_weight(struct layer_t *layer, void *weight)
{

}

struct matrix_t *bn_layer_forward(struct layer_t *layer, struct matrix_t *input_data)
{
    int i;
    int channel_idx;
    float normalized_value;
	struct bn_layer_t *bn_layer = (struct bn_layer_t *)layer->handle;

    if(bn_layer->num_channels != input_data->shape.dims[C])
    {
        printf("input_shape: ");
        shape_print(input_data->shape);

        printf("weight_shape: ");
        shape_print(bn_layer->weights->shape);
        LOG_ERROR("bn wieght or input shape error");
    }
   
    if(NULL == bn_layer->output_data)
    {
        bn_layer->output_data = matrix_alloc_shape(input_data->shape);
    }

    for (i = 0; i < input_data->shape.size; i++) 
	{
        channel_idx = i % bn_layer->num_channels;
        normalized_value = (input_data->data[i] - bn_layer->mean->data[channel_idx]) / sqrtf(bn_layer->variance->data[channel_idx]);
        bn_layer->output_data->data[i] = bn_layer->weights->data[channel_idx] * normalized_value + bn_layer->bias->data[channel_idx];
    }
    

    return bn_layer->output_data;
}

struct matrix_t *bn_layer_backward(struct layer_t *layer, struct matrix_t *input_data)
{

}

//#define BN_LAYER_TEST 1
#if BN_LAYER_TEST
int main(int argc, char *argv[])
{
    int i, j;
    int batch_size = 1;
    int in_channels = 3;
    int in_height = 3;
    int in_width = 3;
	int is_train = 0;

    struct shape_t input_shape;
    input_shape.num_dims = 3;
    input_shape.dims[0] = in_width;
    input_shape.dims[1] = in_height;
    input_shape.dims[2] = in_channels;

    struct layer_t *layer = bn_layer_alloc(&in_channels, &is_train);

    struct bn_layer_t *bn_layer = (struct bn_layer_t *)layer->handle;

    struct bn_weight_t bn_weight;
    struct matrix_t *output_data = NULL;
    struct matrix_t *input_data = NULL;

    float weights[] = {
            1, 1, 1
          };
    float bias[] = {
            0.0, 0.0, 0.0
    };

    bn_weight.weights = matrix_alloc_empty(1, in_channels);
    bn_weight.bias = matrix_alloc_empty(1, in_channels); 

    bn_weight.weights->data = weights;
    bn_weight.bias->data = bias;

    bn_weight.mean = NULL;
    bn_weight.variance = NULL;

    bn_load_weight(layer, &bn_weight);
  
	float input[] = {
            -1.1258, -1.1524, -0.2506,
          -0.4339,  0.8487,  0.6920,
          -0.3160, -2.1152,  0.3223,

         -1.2633,  0.3500,  0.2660,
           0.1665,  0.8744, -0.1435,
          -0.1116, -0.6136,  0.0316,

         -0.4927,  0.0537,  0.6181,
          -0.4128, -0.8411, -2.3160,
          -0.1023,  0.7924, -0.2897 
	};
	input_data = matrix_alloc_empty(1, in_width * in_height * in_channels);
    input_data->data = input;
    
    output_data = bn_layer_forward(layer, input_data);
    matrix_print(output_data);
}
#endif //BN_LAYER_TEST



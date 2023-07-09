#include "base.h"
#include "nn.h"
#include "activation_layer.h"


float relu(float x)
{
    return max(x, (float)0);
}

float relu_df(float x)
{
    return x > (float) 0 ? (float) 1 : (float)0;
}

float p_relu(float x, float alpha)
{
    if(x >= 0)
    {
        return x;
    }
    else
    {
        return alpha * x;
    }
}

float p_relu_df(float x)
{
    //return MAX(x, (float)0);
}

float leaky_relu(float x)
{
    return x > (float) 0 ? x : x * (float) 0.01;
}

float leaky_relu_df(float x)
{
    return x > (float) 0 ? (float)1 : (float) 0.01;
}

float tan_h(float x)
{
    return 1 - x * x;
}

float sigmoid(float x)
{
    return 1 / (1 + exp(-x));
}

float softmax(float x)
{

}

struct layer_t * activation_layer_alloc(void *_activation_name, void *padding_h, void *padding_w, void *offset)
{
    struct layer_t *layer = NULL;
    struct activation_layer_t *activation_layer = NULL;
    char *activation_name = (char *)_activation_name;

    layer = (struct layer_t *)malloc(sizeof(struct layer_t));
    if(NULL == layer)
    {   
        LOG_DEBUG("activation layer malloc size %ld  error %s", sizeof(struct layer_t), strerror(errno));
        return NULL;
    }   

    activation_layer = (struct activation_layer_t *)malloc(sizeof(struct activation_layer_t));
    if(NULL == activation_layer)
    {   
        free(layer);
        LOG_DEBUG("activation_layer handle malloc size %ld  error %s", sizeof(struct activation_layer_t), strerror(errno));
        return NULL;
    }    

    if(STRPREFIX(activation_name, "relu"))
    {
        activation_layer->type = RELU;
        activation_layer->activate = relu;
    }
    else if(STRPREFIX(activation_name, "p_relu"))
    {
        activation_layer->type = PRELU;
        activation_layer->activate = p_relu;
    }
    else if(STRPREFIX(activation_name, "leaky_relu"))
    {
        activation_layer->type = LEAKY_RELU;
        activation_layer->activate = leaky_relu;
    }
    else if(STRPREFIX(activation_name, "tanh"))
    {
        activation_layer->type = TANH;
        activation_layer->activate = tan_h;
    }
    else if(STRPREFIX(activation_name, "sigmoid"))
    {

        activation_layer->type = SIGMOID;
        activation_layer->activate = sigmoid;
    }
    else if(STRPREFIX(activation_name, "softmax"))
    {
        activation_layer->type = SOFTMAX;
        activation_layer->activate = softmax;
    }
    else
    {
        LOG_ERROR("not support activation %s", activation_name);
    }


    //activation_layer->weights = matrix_alloc(1, num_channels);
    //activation_layer->input_shape = *(struct shape_t *)input_shape;
    activation_layer->padding_h = *(int *)padding_h;
    activation_layer->padding_w = *(int *)padding_w;
    activation_layer->offset = *(int *)offset;
    layer->handle = activation_layer;

    return layer;
}

void activation_layer_free(struct layer_t *layer)
{
    struct activation_layer_t *activation_layer = (struct activation_layer_t *)layer->handle;
    if(layer)
    {   
        if(activation_layer)
        {
            free(activation_layer);
        }

        activation_layer = NULL;
        layer->handle = NULL;
        free(layer);
    }   
}

int activation_load_weight(struct layer_t *layer, void *weight)
{
    struct activation_layer_t *activation_layer = (struct activation_layer_t *)layer->handle;
    struct actiation_weight_t *activation_weight = (struct actiation_weight_t *)weight;
    
    if(activation_weight)
    {   
        if(activation_layer->weights)
        {
            matrix_free(activation_layer->weights);
        }
        activation_layer->weights = matrix_copy(activation_weight->weights);
		activation_layer->num_channels = activation_layer->weights->shape.dims[activation_layer->weights->shape.num_dims - 1];
        return SUCCESS;
    }   
    return ERROR;

}

int activation_save_weight(struct layer_t *layer, void *weight)
{

}

matrix_t *padding(struct layer_t *layer, matrix_t *input_data)
{
    struct activation_layer_t *activation_layer = (struct activation_layer_t *)layer->handle;
#if 0
    if(NULL == activation_layer->pad_output_data)
    {
        shape_t shape = input_data->shape;
        shape.dims[0] += activation_layer->padding_h;
        shape.dims[1] += activation_layer->padding_w;
        shape.dims[2] += activation_layer->padding_c;

        activation_layer->pad_output_data = matrix_alloc(shape);

    }
    else if(activation_layer->pad_output_data->shape.dims[0] != 
                input_data->shape.dims[0] + activation_layer->padding_h ||
            activation_layer->pad_output_data->shape.dims[1] != 
                input_data->shape.dims[1] + activation_layer->padding_h||
            activation_layer->pad_output_data->shape.dims[2] != 
                input_data->shape.dims[2])
    {

        matrix_free(activation_layer->pad_output_data);
        shape_t shape = input_data->shape;
        shape.dims[0] += activation_layer->padding_h;
        shape.dims[1] += activation_layer->padding_w;
        shape.dims[2] += activation_layer->padding_c;
        activation_layer->pad_output_data = matrix_alloc(shape);
    }
#endif

    shape_print(input_data->shape);

    // 130
    // 65
    // 35
    //
    //int padded_idx = (c + pad_channels) * new_height * new_width + (h + pad_height) * new_width + (w + pad_width);

    //LOG_DEBUG("pos %d", input_data->shape.dims[1] + activation_layer->padding_h + activation_layer->padding_w);

    LOG_DEBUG("offset %d ", activation_layer->offset);
    matrix_apply_copy(activation_layer->pad_output_data, input_data, activation_layer->offset);
    

    shape_print(activation_layer->pad_output_data->shape);

    return activation_layer->pad_output_data;
}

struct matrix_t *activation_layer_forward(struct layer_t *layer, struct matrix_t *input_data)
{
    int i;
	int channel_idx;
    struct activation_layer_t *activation_layer = (struct activation_layer_t *)layer->handle;
   
    if(activation_layer->num_channels != input_data->shape.dims[input_data->shape.num_dims - 1]) 
    {   
		printf("input_shape: ");
        shape_print(input_data->shape);

        printf("weight_shape: ");
        shape_print(activation_layer->weights->shape);
        LOG_ERROR("activation weight or input  shape error");
    }   
   
    if(NULL == activation_layer->output_data)
    {   
        activation_layer->output_data = matrix_alloc_shape(input_data->shape);
    }   

    for(i = 0; i < input_data->shape.size; i++)
    {
		channel_idx = i % activation_layer->num_channels;
        activation_layer->output_data->data[i] = activation_layer->activate(input_data->data[i], activation_layer->weights->data[channel_idx]);
    }
    LOG_DEBUG("================================== ");
    shape_print(activation_layer->output_data->shape);

	return padding(layer, activation_layer->output_data);
}

struct matrix_t *activation_layer_backward(struct layer_t *layer, struct matrix_t *input_data)
{

}


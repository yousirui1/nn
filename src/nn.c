#include "base.h"
#include "nn.h"
#include "layer.h"
#include "matrix.h"
#include "input_layer.h"
#include "fc_layer.h"
#include "bn_layer.h"
#include "conv_layer.h"
#include "conv_transpose_layer.h"
#include "lstm_layer.h"
#include "activation_layer.h"
#include "data.h"

struct support_layer_t support_layer[] = { 
    {INPUT_LAYER,  "input_layer"},
    {LINEAR_LAYER,  "linear_layer"},
    {BN_LAYER,  "bn_layer"},
    {CONV_LAYER,  "conv_layer"},    
    {CONV_TRANSPOSE_LAYER,  "conv_transpose_layer"},    
    {LSTM_LAYER,  "lstm_layer"},
    {ACTIVATION_LAYER,  "activation_layer"}
};

void nn_weight_save()
{

}

int nn_load_layer_weight(struct nn_t *nn)
{

}

float nn_accurracy()
{

}

float nn_loss()
{

}

struct matrix_t * nn_forward(struct nn_t *nn, struct matrix_t *input_data)
{
    int i, first_flag = 1;
    int ret;
    struct layer_t *layer = NULL;
    struct list_head *pos = NULL;
    struct matrix_t *output_data = NULL;
    for(i = 0; i < nn->batch_size; i++)
    {
        list_for_each(pos, &nn->layer_root)
        {
            layer = list_entry(pos, struct layer_t, link);
            if(first_flag)
            {
	            output_data = layer->forward(layer, input_data);	
                first_flag = 0;
            }
            else
            {
			    LOG_DEBUG("layer_name %s ", layer->layer_name);
                output_data = layer->forward(layer, output_data);
            }
            shape_print(output_data->shape);
            matrix_print(output_data, NULL);
        }
    }
    return output_data;
}

//to do 
void nn_backward()
{
}

void nn_init(struct nn_t *nn, int batch_size, const char *nn_name)
{
    memset(nn, 0, sizeof(struct nn_t));
    INIT_LIST_HEAD(&nn->layer_root);  
    nn->batch_size = batch_size > 0 ? batch_size : 1;

    if(nn_name)
    {
        strncpy(nn->nn_name, nn_name, 128);
    }
}

int nn_add_layer(struct nn_t *nn, int layer_type,  const char *layer_name, int num_params, ...)
{
    int i, ret;
    struct layer_t *layer = NULL;
    va_list args;
    void* params[num_params];

    va_start(args, num_params);

    for(i = 0; i < num_params; i++)
    {
        params[i] = va_arg(args, void *);
    }
    va_end(args);

	switch(layer_type)
    {
        case INPUT_LAYER:
            layer = input_layer_alloc(params[0], params[1]);
            layer->forward = input_layer_forward;
            layer->backward = input_layer_forward;
            layer->free = input_layer_free;
            layer->save_weight = input_save_weight;
            layer->load_weight = input_load_weight;
            break;
        case LINEAR_LAYER:
            layer = linear_layer_alloc(params[0], params[1]);
            layer->forward = linear_layer_forward;
            layer->backward = linear_layer_backward;;
            layer->free = linear_layer_free;
            layer->save_weight = linear_save_weight;
            layer->load_weight = linear_load_weight;
            break;
        case BN_LAYER:
            layer = bn_layer_alloc(params[0]);
            layer->forward = bn_layer_forward;
            layer->backward = bn_layer_backward;;
            layer->free = bn_layer_free;
            layer->save_weight = bn_save_weight;
            layer->load_weight = bn_load_weight;
            break;
        case CONV_LAYER:
            layer = conv_layer_alloc(params[0], params[1], params[2], params[3], params[4], params[5]);
            layer->forward = conv_layer_forward;
            layer->backward = conv_layer_backward;;
            layer->free = conv_layer_free;
            layer->save_weight = conv_save_weight;
            layer->load_weight = conv_load_weight;
            break;
        case CONV_TRANSPOSE_LAYER:
            layer = conv_transpose_layer_alloc(params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
            layer->forward = conv_transpose_layer_forward;
            layer->backward = conv_transpose_layer_backward;;
            layer->free = conv_transpose_layer_free;
            layer->save_weight = conv_transpose_save_weight;
            layer->load_weight = conv_transpose_load_weight;
            break;

        case LSTM_LAYER:
            layer = lstm_layer_alloc(params[0], params[1]);
            layer->forward = lstm_layer_forward;
            layer->backward = lstm_layer_backward;;
            layer->free = lstm_layer_free;
            layer->save_weight = lstm_save_weight;
            layer->load_weight = lstm_load_weight;
            break;
        case ACTIVATION_LAYER:
            layer = activation_layer_alloc(params[0], params[1], params[2], params[3]);
            layer->forward = activation_layer_forward;
            layer->backward = activation_layer_backward;;
            layer->free = activation_layer_free;
            layer->save_weight = activation_save_weight;
            layer->load_weight = activation_load_weight;
            break;
        default:
            LOG_DEBUG("no support op %d ", layer_type);
            return ERROR;
    }

    if(layer_name)
    {
        strncpy(layer->layer_name, layer_name, 128); 
    }
    else
    {
        sprintf(layer->layer_name, "%s_%d", support_layer[layer_type].layer_name, nn->layer_size);
    }
    list_insert_before(&layer->link, &nn->layer_root);
    nn->layer_size ++;
}

void nn_deinit(struct nn_t *nn)
{
    struct layer_t *layer = NULL;
    struct list_head *pos = NULL;
    list_for_each(pos, &nn->layer_root)
    {
        layer = list_entry(pos, struct layer_t, link);
		LOG_DEBUG("layer_name %s ", layer->layer_name);
    }
}


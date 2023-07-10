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
            matrix_print(output_data);
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


#define NN_TEST 1
#if NN_TEST
int main(int argc, char *argv[])
{
	struct nn_t nn;
	struct shape_t input_shape, reshape;
    int out_channel = 4, kernel_height = 2, kernel_width = 5, stride_height = 1, stride_width= 2;
    int padding = 0;
    int activation_type = 2;
    int padding_h = 1, padding_w = 2, padding_c = 0;
    int is_train;
    int offset = 0;     // to do

    matrix_t *input_data = NULL;

    input_shape.num_dims = 4;
    input_shape.dims[N] = 1;
    input_shape.dims[H] = 2;
    input_shape.dims[W] = 257;
    input_shape.dims[C] = 1;

    input_data = matrix_empty_shape(input_shape);
    input_data->data = input;

    nn_init(&nn, 1, "test_nn");

    reshape.num_dims = 4;
    reshape.dims[N] = 1;
    reshape.dims[H] = 2;
    reshape.dims[W] = 257; 
    reshape.dims[C] = 1; 

    /* -------------- block 0 -----------------*/
    nn_add_layer(&nn, INPUT_LAYER, "input", 1, &reshape); 
    out_channel = 4; kernel_height = 2; kernel_width = 5; stride_height = 1; stride_width= 2;
    nn_add_layer(&nn, CONV_LAYER, "conv", 6, &out_channel, &kernel_height, 
                                &kernel_width,&stride_height, &stride_width, &padding); 

    nn_add_layer(&nn, BN_LAYER, "bn", 1, &is_train); 
    padding_h = 1, padding_w = 2;
    offset = 520;
    nn_add_layer(&nn, ACTIVATION_LAYER, "p_relu", 4, "p_relu",  &padding_h, &padding_w, &offset); 

    /* -------------- block 1 -----------------*/

    out_channel = 4; kernel_height = 2; kernel_width = 3; stride_height = 1; stride_width= 2;
    nn_add_layer(&nn, CONV_LAYER, "conv_1", 6,  &out_channel, &kernel_height, 
                            &kernel_width, &stride_height, &stride_width, &padding); 

    nn_add_layer(&nn, BN_LAYER, "bn_1", 1, &out_channel); 
    padding_h = 1, padding_w = 1, padding_c = 0;
    offset = 260;
    nn_add_layer(&nn, ACTIVATION_LAYER, "p_relu_1", 4, "p_relu", &padding_h, &padding_w, &offset);

    /* -------------- block 2 -----------------*/

    out_channel = 4; kernel_height = 2; kernel_width = 3; stride_height = 1; stride_width= 2;
    nn_add_layer(&nn, CONV_LAYER, "conv_2", 6,  &out_channel, &kernel_height, 
                            &kernel_width, &stride_height, &stride_width, &padding); 


    nn_add_layer(&nn, BN_LAYER, "bn_2", 1, &out_channel); 
    padding_h = 1, padding_w = 2, padding_c = 0;
    offset = 140;
    nn_add_layer(&nn, ACTIVATION_LAYER, "p_relu_2", 4, "p_relu", &padding_h, &padding_w, &offset);

    /* -------------- block 3 -----------------*/
    out_channel = 8; kernel_height = 2; kernel_width = 3; stride_height = 1; stride_width= 1;
    nn_add_layer(&nn, CONV_LAYER, "conv_3", 6,  &out_channel, &kernel_height, 
                            &kernel_width, &stride_height, &stride_width, &padding); 

    nn_add_layer(&nn, BN_LAYER, "bn_3", 1, &out_channel); 
    padding_h = 1, padding_w = 2;
    nn_add_layer(&nn, ACTIVATION_LAYER, "p_relu_3", 4, "p_relu", &padding_h, &padding_w, &offset);


    /* -------------- block 4 -----------------*/
    out_channel = 8; kernel_height = 2; kernel_width = 3; stride_height = 1; stride_width= 1;
    nn_add_layer(&nn, CONV_LAYER, "conv_4", 6,  &out_channel, &kernel_height, 
                            &kernel_width, &stride_height, &stride_width, &padding); 

    nn_add_layer(&nn, BN_LAYER, "bn_4", 1, &out_channel); 
    padding_h = 0, padding_w = 0;
    nn_add_layer(&nn, ACTIVATION_LAYER, "p_relu_4", 4, "p_relu", &padding_h, &padding_w, &offset);


#if 0
    /* ----------  block 5 -------------------- */
     out_channel = 8; kernel_height = 2; kernel_width = 3; stride_height = 1; stride_width= 1;
    nn_add_layer(&nn, CONV_LAYER, "conv_transpose", 6,  &out_channel, &kernel_height, 
                            &kernel_width, &stride_height, &stride_width, &padding); 

    nn_add_layer(&nn, BN_LAYER, "bn_5", 1, &out_channel); 
    padding_h = 0, padding_w = 0;
    nn_add_layer(&nn, ACTIVATION_LAYER, "p_relu_5", 3, "p_relu", &padding_h, &padding_w);

    /* ----------  block 6 -------------------- */
     out_channel = 8; kernel_height = 2; kernel_width = 3; stride_height = 1; stride_width= 1;
    nn_add_layer(&nn, CONV_LAYER, "conv_transpose_1", 6,  &out_channel, &kernel_height, 
                            &kernel_width, &stride_height, &stride_width, &padding); 

    nn_add_layer(&nn, BN_LAYER, "bn_6", 1, &out_channel); 
    padding_h = 0, padding_w = 0;
    nn_add_layer(&nn, ACTIVATION_LAYER, "p_relu_6", 3, "p_relu", &padding_h, &padding_w);


    /* ----------  block 7 -------------------- */
    out_channel = 4; kernel_height = 2; kernel_width = 3; stride_height = 1; stride_width= 2;
    nn_add_layer(&nn, CONV_LAYER, "conv_transpose_2", 6,  &out_channel, &kernel_height, 
                            &kernel_width, &stride_height, &stride_width, &padding); 

    nn_add_layer(&nn, BN_LAYER, "bn_7", 1, &out_channel); 
    padding_h = 0, padding_w = 0;
    nn_add_layer(&nn, ACTIVATION_LAYER, "p_relu_7", 3, "p_relu", &padding_h, &padding_w);


    /* ----------  block 8 -------------------- */
    out_channel = 4; kernel_height = 2; kernel_width = 3; stride_height = 1; stride_width= 2;
    nn_add_layer(&nn, CONV_LAYER, "conv_transpose_2", 6,  &out_channel, &kernel_height, 
                            &kernel_width, &stride_height, &stride_width, &padding); 

    // decode 
    nn_add_layer(&nn, BN_LAYER, "bn_8", 1, &out_channel); 
    padding_h = 0, padding_w = 0;
    nn_add_layer(&nn, ACTIVATION_LAYER, "p_relu_8", 3, "p_relu", &padding_h, &padding_w);

    out_channel = 1; kernel_height = 2; kernel_width = 5; stride_height = 1; stride_width= 2;
    nn_add_layer(&nn, CONV_LAYER, "conv_transpose", 6,  &out_channel, &kernel_height, 
#endif

    //nn_load_layer_weight(&nn);
    nn_load_weight(&nn);

    nn_forward(&nn, input_data);
    //nn_loss(&nn);
}
#endif //NN_TEST

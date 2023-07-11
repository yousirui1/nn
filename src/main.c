#include "base.h"
#include "matrix.h"


//#define NN_TEST 1
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



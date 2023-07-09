#include "base.h"
#include "conv_layer.h"
#include "gemm.h"
#include "im2col.h"

struct layer_t * conv_layer_alloc(void* out_channels, void *kernel_height, void* kernel_width, void *stride_height, 
                                    void *stride_width,  void *padding)
{
    struct layer_t *layer = NULL;
    struct conv_layer_t *conv_layer = NULL;

    layer = (struct layer_t *)malloc(sizeof(struct layer_t));
    if(NULL == layer)
    {   
        LOG_DEBUG("bn layer malloc size %ld  error %s", sizeof(struct layer_t), strerror(errno));
        return NULL;
    }   

    conv_layer = (struct conv_layer_t *)malloc(sizeof(struct conv_layer_t));
    if(NULL == conv_layer)
    {   
        free(layer);
        LOG_DEBUG("conv_layer handle malloc size %ld  error %s", sizeof(struct conv_layer_t), strerror(errno));
        return NULL;
    }     

    conv_layer->out_channels =  *(int *)out_channels;
    conv_layer->kernel_w = *(int *)kernel_width;
    conv_layer->kernel_h = *(int *)kernel_height;
    conv_layer->stride_w = *(int *)stride_width;
    conv_layer->stride_h = *(int *)stride_height;;
    conv_layer->padding = *(int *)padding;

	//conv_layer->weights = matrix_alloc(conv_layer->out_channels, conv_layer->col.dims[2]);
    //conv_layer->bias = matrix_alloc(conv_layer->out_channels, 1);

    // batch_size default 1
    //conv_layer->output_data = matrix_alloc(1, conv_layer->output_size);
    layer->handle = conv_layer;
	return layer;
}

void conv_layer_free(struct layer_t *layer)
{
    struct conv_layer_t *conv_layer = (struct conv_layer_t *)layer->handle;
    if(layer)
    {
        if(conv_layer)
        {
            free(conv_layer);
        }

        conv_layer = NULL;
        layer->handle = NULL;
        free(layer);
    }
}

int conv_load_weight(struct layer_t *layer, void *weight)
{
    struct conv_layer_t *conv_layer = (struct conv_layer_t *)layer->handle;
    struct conv_weight_t *conv_weight = (struct conv_weight_t *)weight;

    if(conv_weight)
    {
        if(conv_weight->weights && conv_weight->bias)
        {
            if(conv_layer->weights)
            {
                matrix_free(conv_layer->weights);
            }

            conv_layer->weights = matrix_copy(conv_weight->weights);
            //matrix_print(conv_layer->weights);
            if(conv_layer->bias)
            {
                matrix_free(conv_layer->bias);
            }
            conv_layer->bias = matrix_copy(conv_weight->bias);
            //matrix_print(conv_layer->bias);
            return SUCCESS;
        }
    }
    return ERROR;
}

int conv_save_weight(struct layer_t *layer, void *weight)
{

}

void add_biases(float* out, float* biases, int out_c, int out_cols) {
    for (int i = 0; i < out_c; i++) 
    {
        float bias = biases[i];
        for (int j = 0; j < out_cols; j++) 
        {
            out[i * out_cols + j] += bias;
        }
    }   
}


void transpose_weights(float *weights, int channels, int size_h, int size_w)
{
    float *tmp = (float *)calloc(size_h * size_w * channels, sizeof(float));
    for (int i = 0; i < size_h; ++i) {
        for (int j = 0; j < size_w; ++j) {
            for (int k = 0; k < channels; ++k) {
                int dst_index = i * size_w * channels + j * channels + k;
                int src_index = k * size_h * size_w + i * size_w + j;
                tmp[dst_index] = weights[src_index];
            }
        }
    }
    memcpy(weights, tmp, size_h * size_w * channels * sizeof(float));
    free(tmp);
}

struct matrix_t *conv_layer_forward(struct layer_t *layer, struct matrix_t *input_data)
{
    int i;
    struct conv_layer_t *conv_layer = (struct conv_layer_t *)layer->handle;

    int batch_size = input_data->shape.dims[N];
    int input_channels = input_data->shape.dims[C];
    int input_height = input_data->shape.dims[H];
    int input_width = input_data->shape.dims[W];
    int kernel_h = conv_layer->kernel_h;
    int kernel_w = conv_layer->kernel_w;
    int stride_h = conv_layer->stride_h;
    int stride_w = conv_layer->stride_w;
    int padding = conv_layer->padding;
    int height_col = 0;
    int weight_col = 0;
    int channel_col = 0;
    int output_channels = 0;
    int output_height = 0;
    int output_width = 0;

    shape_print(input_data->shape);
    if(NULL == conv_layer->output_data)
    {
        output_channels = conv_layer->out_channels;
        output_height = ((input_height - kernel_h + (2 * padding)) / stride_h) + 1;
        output_width = ((input_width - kernel_w + (2 * padding)) / stride_w) + 1;

        conv_layer->output_data = matrix_alloc(4, batch_size,  output_height, 
                output_width, output_channels);
    }
    else
    {
        output_height = conv_layer->output_data->shape.dims[H];
        output_width = conv_layer->output_data->shape.dims[W];
        output_channels = conv_layer->output_data->shape.dims[C];
    }
    LOG_DEBUG("output: ");
    shape_print(conv_layer->output_data->shape);

    if(NULL == conv_layer->col_data)  
    {
        height_col = (input_height + 2 * padding - kernel_h) / stride_h + 1;
        weight_col = (input_width + 2 * padding - kernel_w) / stride_w + 1;
        channel_col = input_channels * kernel_w  * kernel_h; 
        conv_layer->col_data = matrix_alloc(4, batch_size,  channel_col, height_col, weight_col);
    }
    else
    {
        height_col = conv_layer->col_data->shape.dims[H];
        weight_col = conv_layer->col_data->shape.dims[W];
        channel_col = conv_layer->col_data->shape.dims[C];
    }
    LOG_DEBUG("col_data: ");
    shape_print(conv_layer->col_data->shape);


    int m = output_channels;
    int k = kernel_h * kernel_w * input_channels;
    int n = output_height * output_width;

    float *a = conv_layer->weights->data;
    float *b = conv_layer->col_data->data;
    float *c = conv_layer->output_data->data;

	//transpose_weights(a, output_channels, kernel_h, kernel_w);

    for(i = 0; i < batch_size; i++)
    {
        im2col_cpu(input_data, kernel_h, kernel_w, stride_h, stride_w, padding,
                    conv_layer->col_data);
        
        gemm(0, 0, m, n, k, 1, a, k, b, n, 1, c, n);
		add_biases(c, conv_layer->bias->data, conv_layer->out_channels, n);
    }

    return conv_layer->output_data;
}

struct matrix_t *conv_layer_backward(struct layer_t *layer, struct matrix_t *input_data)
{

}

//#define CONV_LAYER_TEST 1
#if CONV_LAYER_TEST

float test_weights[] = {
        -0.1374,  0.1839,  0.0117, -0.0274,
        0.1149,  0.1314, -0.2379,  0.1175,
        0.2222, -0.0515,  0.0503,  0.1095,
        -0.1888, -0.2117,  0.2460,  0.1885
};


float test_bias[] = {
    -0.1826
};

float test_input[] = {
           1.8496,  0.6440,  1.8863, -0.9232,  1.2570,  0.1653, -1.4051,
            0.9328, -1.1276,  1.5433, -1.2222,  0.7369,
          -0.8201,  1.1903, -0.0872, -1.0456,  0.2779, -0.9526,  0.3937,
            0.6626,  0.3731, -1.7594, -0.2173,  2.3448,
           0.6468,  0.0724, -0.2813, -0.9134,  1.3253, -0.9750, -0.5888,
            0.0366,  1.4338,  0.1522, -0.4161,  0.2330,
          -0.1868, -0.3413,  0.9265,  0.2403,  0.3880,  1.7778, -0.1773,
           -0.1546, -0.2764,  0.6062, -0.4279, -0.0514,
           0.2716,  1.7108, -2.6756, -0.6172,  0.4249, -0.1789,  0.7449,
            0.3631, -1.0002, -0.3082,  0.6297, -0.3403,
          -1.4525,  1.3841, -0.9330,  0.0872,  1.2905, -1.8060, -1.0008,
            2.5474,  0.5491, -1.3234, -0.3675, -0.8582,
           1.0249, -0.8594,  1.8487,  1.1436,  0.4251, -0.0162, -1.2786,
            0.7256,  0.6194,  0.8831,  1.4020, -0.7497,
           0.5158,  1.0888, -0.8781, -1.0277, -0.0398, -0.2209,  0.4943,
           -0.4950,  0.6110,  1.0321,  1.2444, -1.2125,
          -0.4228,  1.0216, -0.6661, -0.3224, -0.6758, -1.1899,  0.2245,
           -1.2934,  0.3023,  0.1354, -0.9931, -1.5741,
           0.7252,  0.3070, -0.3340,  0.9870, -0.4155,  0.1101, -0.1147,
           -0.5349, -0.2674,  1.8996,  0.6675,  0.7294,
           2.3711,  0.1948, -0.5056,  0.3801,  1.3129,  0.7153, -0.5340,
            2.3752, -0.3654,  0.2949, -1.6020,  0.4584,
           1.0203,  0.2399, -2.1363,  1.8438, -0.8547, -0.9729, -2.0081,
           -0.4004, -0.5078,  0.0438,  0.8820,  1.9517 
};

int main(int argc, char *argv[])
{
    int i, j;
    int batch_size = 1;

    // tf (batch_size, height, width, channels)
    // torch(batch_size, channels, height, width)
    // darknet(batch_size, channels, height, width)

    int out_channels = 2;
    int kernel_h = 3;
    int kernel_w = 3;
    int stride_h = 2;
    int stride_w = 1;
    int padding = 0;

	struct shape_t input_shape;
	shape_t weight_shape, bias_shape;

    struct layer_t *layer = conv_layer_alloc(&out_channels, &kernel_h, &kernel_w, &stride_h, 
			&stride_w, &padding);

    struct conv_layer_t *conv_layer = (struct conv_layer_t *)layer->handle;

    struct conv_weight_t conv_weight;
    struct matrix_t *output_data = NULL;
    struct matrix_t *input_data = NULL;

	input_shape.num_dims = 3;
    input_shape.dims[0] = 3;    //c
    input_shape.dims[1] = 5;    //h
    input_shape.dims[2] = 5;    //w

    //input_data = matrix_alloc_empty(input_shape);
    //input_data->data = input;
    input_data = matrix_alloc(input_shape);
	//memset(input_data->data, 1, input_data->shape.size * sizeof(float));	
    for(int i = 0; i < input_data->shape.size; i++)
    {
        input_data->data[i] = 0.1 * i;
        //input_data->data[i] = 1.0f;
    }

    matrix_print(input_data);

    weight_shape.num_dims = 4;
    weight_shape.dims[0] = 2;
    weight_shape.dims[1] = 2;
    weight_shape.dims[2] = 3;
    weight_shape.dims[3] = 2;

    //conv_weight.weights = matrix_alloc_empty(weight_shape);
    //conv_weight.weights->data = weights;
    conv_weight.weights = matrix_alloc(weight_shape);
	//memset(conv_weight.weights->data, 1, conv_weight.weights->shape.size * sizeof(float));	
    for(int i = 0; i < conv_weight.weights->shape.size; i++)
    {
        conv_weight.weights->data[i] = 0.1 * i;
        //conv_weight.weights->data[i] = 1.0f;
    }

    bias_shape.num_dims = 1;
    bias_shape.dims[0] = 2;

    //conv_weight.bias = matrix_alloc_empty(bias_shape);
    //conv_weight.bias->data = bias;

    conv_weight.bias = matrix_alloc(bias_shape);
	//memset(conv_weight.bias->data, 1, conv_weight.bias->shape.size * sizeof(float));	
	for(int i = 0; i < conv_weight.bias->shape.size; i++)
    {
        conv_weight.bias->data[i] = 1.0f;
    }

    matrix_print(input_data);
    matrix_print(conv_weight.weights);
    matrix_print(conv_weight.bias);

    conv_load_weight(layer, &conv_weight);
    
    output_data = conv_layer_forward(layer, input_data);

    //printf("output_dims %d %d \n", output_data->rows, output_data->cols);
    shape_print(output_data->shape);
    for(int i = 0; i < output_data->shape.size; i++)
    {
        printf("%f ", output_data->data[i]);
    }
    printf("\n");

}
#endif


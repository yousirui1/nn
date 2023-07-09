#include "base.h"
#include "conv_transpose_layer.h"
#include "gemm.h"

#if 0
struct layer_t *conv_transpose_layer_alloc(void* input_shape, void* out_channels, 
                        void* kernel_h, void *kernel_w, void* stride_h, void *stride_, 
                        void* padding, void *output_padding)
{
    struct layer_t *layer = NULL;
    struct conv_transpose_layer_t *conv_transpose_layer = NULL;

    layer = (struct layer_t *)malloc(sizeof(struct layer_t));
    if(NULL == layer)
    {   
        LOG_DEBUG("bn layer malloc size %ld  error %s", sizeof(struct layer_t), strerror(errno));
        return NULL;
    }   

    conv_transpose_layer = (struct conv_transpose_layer_t *)malloc(sizeof(struct conv_transpose_layer_t));
    if(NULL == conv_transpose_layer)
    {   
        free(layer);
        LOG_DEBUG("conv_transpose_layer handle malloc size %ld  error %s", sizeof(struct conv_transpose_layer_t), strerror(errno));
        return NULL;
    }     

    conv_transpose_layer->input_shape = *(struct shape_t *)input_shape;
    conv_transpose_layer->out_channels = *(int *)out_channels;
    conv_transpose_layer->kernel_h = *(int *)kernel_h;
    conv_transpose_layer->kernel_w = *(int *)kernel_w;
    conv_transpose_layer->stride_h = *(int *)stride_h;
    conv_transpose_layer->stride_w = *(int *)stride_w;
    conv_transpose_layer->padding = *(int *)padding;
    conv_transpose_layer->output_padding = *(int *)output_padding;

 	//conv_transpose_layer->weights = matrix_alloc(conv_transpose_layer->out_channels, conv_transpose_layer->col.dims[2]);
    
    //conv_transpose_layer->bias = matrix_alloc(conv_transpose_layer->out_channels, 1);

    //conv_transpose_layer->output_data = matrix_alloc(1, conv_transpose_layer->output_size);

	//conv_transpose_layer->col.dims[2] = 

    layer->handle = conv_transpose_layer;
	return layer;
}

void conv_transpose_layer_free(struct layer_t *layer)
{
    struct conv_transpose_layer_t *conv_transpose_layer = (struct conv_transpose_layer_t *)layer->handle;
    if(layer)
    {
        if(conv_transpose_layer)
        {
            free(conv_transpose_layer);
        }

        conv_transpose_layer = NULL;
        layer->handle = NULL;
        free(layer);
    }
}

int conv_transpose_load_weight(struct layer_t *layer, void *weight)
{
    struct conv_transpose_layer_t *conv_transpose_layer = (struct conv_transpose_layer_t *)layer->handle;
    struct conv_transpose_weight_t *conv_transpose_weight = (struct conv_transpose_weight_t *)weight;

    if(conv_transpose_weight)
    {
        if(conv_transpose_weight->weights && conv_transpose_weight->bias)
        {
            if(conv_transpose_layer->weights)
            {
                matrix_free(conv_transpose_layer->weights);
            }

            conv_transpose_layer->weights = matrix_copy(conv_transpose_weight->weights);
            if(conv_transpose_layer->bias)
            {
                matrix_free(conv_transpose_layer->bias);
            }
            conv_transpose_layer->bias = matrix_copy(conv_transpose_weight->bias);
            return SUCCESS;
        }
    }
    return ERROR;
}

int conv_transpose_save_weight(struct layer_t *layer, void *weight)
{

}

static void add_biases(float* out, float* biases, int out_c, int out_cols) {
    for (int i = 0; i < out_c; i++) {
        float bias = biases[i];
        for (int j = 0; j < out_cols; j++) {
            out[i * out_cols + j] += bias;
        }
    }   
}

struct matrix_t *conv_transpose_layer_forward(struct layer_t *layer, struct matrix_t *input_data)
{
    //int batch, in_channel, out_channel, out_h, out_w, in_h, in_w, kernel_h, kernel_w;
    int batch_size = 1;
    struct conv_transpose_layer_t *conv_transpose_layer = (struct conv_transpose_layer_t *)layer->handle;
    int input_h = conv_transpose_layer->col.dims[1];
    int input_w = conv_transpose_layer->col.dims[0];
    int input_c = conv_transpose_layer->col.dims[1];

    int output_h = conv_transpose_layer->col.dims[1];
    int output_w = conv_transpose_layer->col.dims[0];
    int output_c = conv_transpose_layer->col.dims[1];

    int kernel_w = conv_transpose_layer->kernel_size;
    int kernel_h = conv_transpose_layer->kernel_size;

    int stride_h = conv_transpose_layer->stride_w;
    int stride_w = conv_transpose_layer->stride;
    int padding = conv_transpose_layer->padding;
    int output_padding = conv_transpose_layer->output_padding;


	return output;
}

struct matrix_t *conv_transpose_layer_backward(struct layer_t *layer, struct matrix_t *input_data)
{

}

//#define CONV_TRANSPOSE_TEST 1
#if CONV_TRANSPOSE_TEST
int main(int argc, char *argv[])
{
    int i, j;
    int batch_size = 1;
    int in_channels = 1;
    int in_height = 3; 
    int in_width = 3; 
    int out_channels = 2;
    int kernel_size = 3;
    int stride = 2;
    int padding = 1;
    int output_padding = 1;

	struct shape_t input_shape;
	input_shape.num_dims = 3;
    input_shape.dims[0] = 3;
    input_shape.dims[1] = 3;
    input_shape.dims[2] = 1;


    struct layer_t *layer = conv_transpose_layer_alloc(&input_shape, &out_channels, &kernel_size, 
            &stride, &padding, &output_padding);

    struct conv_transpose_layer_t *conv_transpose_layer = (struct conv_transpose_layer_t *)layer->handle;

    struct conv_transpose_weight_t conv_transpose_weight;
    struct matrix_t *output_data = NULL;
    struct matrix_t *input_data = NULL;

    float input[] = {
        1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0
    };

    float bias[] = {
            0.0, 0.0
    };

    float weights[] = {
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0
	};

    conv_transpose_weight.weights = matrix_alloc_empty(18, 1);
    conv_transpose_weight.bias = matrix_alloc_empty(conv_transpose_layer->out_channels, 1);

    conv_transpose_weight.weights->data = weights;
    conv_transpose_weight.bias->data = bias;


    matrix_print(conv_transpose_weight.weights);


    conv_load_weight(layer, &conv_transpose_weight);
    
    input_data = matrix_alloc_empty(1, in_width * in_height);
    input_data->data = input;

    output_data = conv_transpose_layer_forward(layer, input_data);

    for(int i = 0; i < conv_transpose_weight.weights->rows * conv_transpose_weight.weights->cols; i++)
    {   
        printf("index %d : %f \n",i , conv_transpose_weight.weights->data[i]);
    }  

    printf("output_dims %d %d \n", output_data->rows, output_data->cols);
    for(int i = 0; i < output_data->rows * output_data->cols; i++)
    {
        if(i % 10 == 0)
            printf("\n");
        printf("%.1f ", output_data->data[i]);
    }
    printf("\n");

}
#endif





































    

#if 0
    conv_transpose_layer->col.num_dims = 3;	//default 2d 

	conv_transpose_layer->col.dims[0] = ((conv_transpose_layer->input_shape.dims[0] - conv_transpose_layer->kernel_size + 
                                  (2 * conv_transpose_layer->padding)) / conv_transpose_layer->stride) + 1;
	conv_transpose_layer->col.dims[1] = ((conv_transpose_layer->input_shape.dims[1] - conv_transpose_layer->kernel_size + 
                                  (2 * conv_transpose_layer->padding)) / conv_transpose_layer->stride) + 1;
	conv_transpose_layer->col.dims[2] = conv_transpose_layer->input_shape.dims[2] * conv_transpose_layer->kernel_size * 
                                    conv_transpose_layer->kernel_size;

    conv_transpose_layer->col_size = conv_transpose_layer->col.dims[0] * conv_transpose_layer->col.dims[1] * conv_transpose_layer->col.dims[2];
    conv_transpose_layer->input_size = conv_transpose_layer->input_shape.dims[0] * conv_transpose_layer->input_shape.dims[1] * conv_transpose_layer->input_shape.dims[2];;

    conv_transpose_layer->output_size = conv_transpose_layer->col.dims[0] * conv_transpose_layer->col.dims[1] * conv_transpose_layer->out_channels;

    // batch_size default 1
    conv_transpose_layer->input_col = matrix_alloc(1, conv_transpose_layer->col_size);
#endif


#if 0
struct matrix_t *conv_transpose_layer_forward(struct layer_t *layer, struct matrix_t *input_data)
{
#if 0
    int i;
    struct conv_transpose_layer_t *conv_transpose_layer = (struct conv_transpose_layer_t *)layer->handle;
    int out_cols = conv_transpose_layer->col.dims[0] * conv_transpose_layer->col.dims[1];

    printf("out_cols %d \n", out_cols);

    float *in_row = NULL, *col_row = NULL, *out_row = NULL;
    for(i = 0; i < input_data->rows; i++)
    {
        in_row = input_data->data + i * conv_transpose_layer->input_size;
        col_row = conv_transpose_layer->input_col->data + i * conv_transpose_layer->col_size;
        out_row = conv_transpose_layer->output_data->data +  i * conv_transpose_layer->output_size;

        iam2cool(in_row, conv_transpose_layer->input_shape.dims[2], conv_transpose_layer->input_shape.dims[0],
                conv_transpose_layer->input_shape.dims[1], conv_transpose_layer->kernel_size, conv_transpose_layer->stride,
                conv_transpose_layer->padding, conv_transpose_layer->col.dims[0], conv_transpose_layer->col.dims[1], 
                conv_transpose_layer->col.dims[2], col_row);

        gemm(false, false, conv_transpose_layer->weights->rows, out_cols, conv_transpose_layer->weights->cols,
                conv_transpose_layer->weights->data, col_row, 0.0f, out_row);

		add_biases(out_row, conv_transpose_layer->bias->data, conv_transpose_layer->out_channels, out_cols);
    }
   return conv_transpose_layer->output_data;
#endif
}
#endif
#endif

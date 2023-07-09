#include "base.h"
#include "lstm_layer.h"
#include "activation_layer.h"

struct layer_t *lstm_layer_alloc(void *input_size, void *hidden_size)
{
    struct layer_t *layer = NULL;
    struct lstm_layer_t *lstm_layer = NULL;

    layer = (struct layer_t *)malloc(sizeof(struct layer_t));
    if(NULL == layer)
    {   
        LOG_DEBUG("lstm layer malloc size %ld  error %s", sizeof(struct layer_t), strerror(errno));
        return NULL;
    }   

    lstm_layer = (struct lstm_layer_t *)malloc(sizeof(struct lstm_layer_t));
    if(NULL == lstm_layer)
    {   
        free(layer);
        LOG_DEBUG("lstm_layer handle malloc size %ld  error %s", sizeof(struct lstm_layer_t), strerror(errno));
        return NULL;
    }     
#if 0
    lstm_layer->input_size = *(int *)input_size;
    lstm_layer->hidden_size = *(int *)hidden_size;

    lstm_layer->prev_hidden_state = matrix_alloc(1, lstm_layer->hidden_size);
    lstm_layer->prev_cell_state = matrix_alloc(1, lstm_layer->hidden_size);

    lstm_layer->hidden_state = matrix_alloc(1, lstm_layer->hidden_size);
    lstm_layer->cell_state = matrix_alloc(1, lstm_layer->hidden_size);

    lstm_layer->num_weights = 4 * lstm_layer->hidden_size * (lstm_layer->hidden_size + lstm_layer->input_size);

    lstm_layer->weight_ih = matrix_alloc(1, lstm_layer->num_weights);
    lstm_layer->weight_hh = matrix_alloc(1, lstm_layer->num_weights);

    lstm_layer->bias_ih = matrix_alloc(1, lstm_layer->hidden_size);
    lstm_layer->bias_hh = matrix_alloc(1, lstm_layer->hidden_size);
#endif

    layer->handle = lstm_layer;

    return layer;
}

void lstm_layer_free(struct layer_t *layer)
{
    struct lstm_layer_t *lstm_layer = (struct lstm_layer_t *)layer->handle;
    if(layer)
    {
        if(lstm_layer)
        {
            free(lstm_layer);
        }

        lstm_layer = NULL;
        layer->handle = NULL;
        free(layer);
    }
}

int lstm_load_weight(struct layer_t *layer, void *weight)
{
#if 0
    struct lstm_layer_t *lstm_layer = (struct lstm_layer_t *)layer->handle;
    struct lstm_weight_t *lstm_weight = (struct lstm_weight_t *)weight;

    if(lstm_weight)
    {   
        if(lstm_layer->prev_hidden_state)
        {
            matrix_free(lstm_layer->prev_hidden_state);
        }
        lstm_layer->prev_hidden_state = matrix_copy(lstm_weight->prev_hidden_state);

   	 	if(lstm_layer->prev_cell_state)
        {
            matrix_free(lstm_layer->prev_cell_state);
        }
        lstm_layer->prev_cell_state = matrix_copy(lstm_weight->prev_cell_state);

   	 	if(lstm_layer->hidden_state)
        {
            matrix_free(lstm_layer->hidden_state);
        }
        lstm_layer->hidden_state = matrix_copy(lstm_weight->hidden_state);

   	 	if(lstm_layer->cell_state)
        {
            matrix_free(lstm_layer->cell_state);
        }
        lstm_layer->cell_state = matrix_copy(lstm_weight->cell_state);

   	 	if(lstm_layer->weight_ih)
        {
            matrix_free(lstm_layer->weight_ih);
        }
        lstm_layer->weight_ih = matrix_copy(lstm_weight->weight_ih);

   	 	if(lstm_layer->weight_hh)
        {
            matrix_free(lstm_layer->weight_hh);
        }
        lstm_layer->weight_hh = matrix_copy(lstm_weight->weight_hh);

   	 	if(lstm_layer->bias_ih)
        {
            matrix_free(lstm_layer->bias_ih);
        }
        lstm_layer->bias_ih = matrix_copy(lstm_weight->bias_ih);

   	 	if(lstm_layer->bias_hh)
        {
            matrix_free(lstm_layer->bias_hh);
        }
        lstm_layer->bias_hh = matrix_copy(lstm_weight->bias_hh);
        return SUCCESS;
    }   
    return ERROR;
#endif

}

int lstm_save_weight(struct layer_t *layer, void *weight)
{

}

struct matrix_t *lstm_layer_forward(struct layer_t *layer, struct matrix_t *input_data)
{
#if 0
    int i;
    struct lstm_layer_t *lstm_layer = (struct lstm_layer_t *)layer->handle;

    float *input = input_data->data;

    float *prev_hidden_state = lstm_layer->prev_hidden_state->data;
    float *prev_cell_state = lstm_layer->prev_cell_state->data;

    float *weight_ih = lstm_layer->weight_ih->data;
    float *weight_hh = lstm_layer->weight_hh->data;

    float *bias_ih = lstm_layer->bias_ih->data;
    float *bias_hh = lstm_layer->bias_hh->data;

    float *hidden_state = lstm_layer->hidden_state->data;
    float *cell_state = lstm_layer->cell_state->data;

    int hidden_size = lstm_layer->hidden_size;
    int input_size = lstm_layer->input_size;


    float input_gate = 0.0;
    float forget_gate = 0.0;
    float output_gate = 0.0;
    float candidate_cell_state = 0.0;
    for (int i = 0; i < hidden_size; ++i) {
        input_gate += weight_ih[i * input_size] * input[i] + weight_hh[i * hidden_size] * prev_hidden_state[i] + bias_ih[i] + bias_hh[i];

        forget_gate += weight_ih[(hidden_size + i) * input_size] * input[i] + weight_hh[(hidden_size + i) * hidden_size] * prev_hidden_state[i] + bias_ih[hidden_size + i] + bias_hh[hidden_size + i];

        output_gate += weight_ih[(2 * hidden_size + i) * input_size] * input[i] + weight_hh[(2 * hidden_size + i) * hidden_size] * prev_hidden_state[i] + bias_ih[2 * hidden_size + i] + bias_hh[2 * hidden_size + i];

        candidate_cell_state += weight_ih[(3 * hidden_size + i) * input_size] * input[i] + weight_hh[(3 * hidden_size + i) * hidden_size] * prev_hidden_state[i] + bias_ih[3 * hidden_size + i] + bias_hh[3 * hidden_size + i];
    }

    input_gate = sigmoid(input_gate);
    forget_gate = sigmoid(forget_gate);
    output_gate = sigmoid(output_gate);
    candidate_cell_state = tanh(candidate_cell_state);

    // 计算当前时刻的记忆细胞和隐藏状态
    for (int i = 0; i < hidden_size; ++i) {
        cell_state[i] = forget_gate * prev_cell_state[i] + input_gate * candidate_cell_state;
        hidden_state[i] = output_gate * tanh(cell_state[i]);

    }

    return lstm_layer->hidden_state;
#endif
}

struct matrix_t *lstm_layer_backward(struct layer_t *layer, struct matrix_t *input_data)
{

}


#if 0
int main(int argc, char *argv[])
{
	int input_size = 10;
    int hidden_size = 2;

    // 初始化输入、隐藏状态和细胞状态
    float input[input_size];
    float prev_hidden_state[hidden_size];
    float prev_cell_state[hidden_size];
    float hidden_state[hidden_size];
    float cell_state[hidden_size];
    for (int i = 0; i < input_size; ++i) {
        input[i] = 0.5;
    }   
    for (int i = 0; i < hidden_size; ++i) {
        prev_hidden_state[i] = 0.0;
        prev_cell_state[i] = 0.0;
        hidden_state[i] = 0.0;
        cell_state[i] = 0.0;
    }   

    // 定义权重和偏置
    int num_weights = 4 * hidden_size * (input_size + hidden_size);
    float* weight_ih = malloc(num_weights * sizeof(float));
    float* weight_hh = malloc(num_weights * sizeof(float));
    float* bias_ih = malloc(4 * hidden_size * sizeof(float));
    float* bias_hh = malloc(4 * hidden_size * sizeof(float));
    for (int i = 0; i < num_weights; ++i) {
        weight_ih[i] = 0.1;
        weight_hh[i] = 0.2;
    }   
    for (int i = 0; i < 4 * hidden_size; ++i) {
        bias_ih[i] = 0.3;
        bias_hh[i] = 0.4;
    }  

    struct layer_t *layer = lstm_layer_alloc(&input_size, &hidden_size);
    struct lstm_layer_t *lstm_layer = (struct lstm_layer_t *)layer->handle;

    struct lstm_weight_t lstm_weight;
    struct matrix_t *output_data = NULL;
    struct matrix_t *input_data = NULL;

    lstm_weight.prev_hidden_state = matrix_alloc_empty(1, lstm_layer->hidden_size);
    lstm_weight.prev_hidden_state->data = prev_hidden_state;
	
    lstm_weight.prev_cell_state = matrix_alloc_empty(1, lstm_layer->hidden_size);
    lstm_weight.prev_cell_state->data = prev_cell_state;

    lstm_weight.hidden_state = matrix_alloc_empty(1, lstm_layer->hidden_size);
    lstm_weight.hidden_state->data = hidden_state;

    lstm_weight.cell_state = matrix_alloc_empty(1, lstm_layer->hidden_size);
    lstm_weight.cell_state->data = cell_state;

    lstm_weight.weight_ih = matrix_alloc_empty(1, lstm_layer->num_weights);
    lstm_weight.weight_ih->data = weight_ih;

    lstm_weight.weight_hh = matrix_alloc_empty(1, lstm_layer->num_weights);
    lstm_weight.weight_hh->data = weight_hh;

    lstm_weight.bias_ih = matrix_alloc_empty(1, 4 * lstm_layer->hidden_size);
    lstm_weight.bias_ih->data = bias_ih;

    lstm_weight.bias_hh = matrix_alloc_empty(1, 4 * lstm_layer->hidden_size);
    lstm_weight.bias_hh->data = bias_hh;


    lstm_load_weight(layer, &lstm_weight);

    input_data = matrix_alloc_empty(1, input_size);
    input_data->data = input;

    output_data = lstm_layer_forward(layer, input_data);

    printf("output_dims %d %d \n", output_data->rows, output_data->cols);
    for(int i = 0; i < output_data->rows * output_data->cols; i++)
    {
        printf("%f ", output_data->data[i]);
    }
    printf("\n");
}
#endif

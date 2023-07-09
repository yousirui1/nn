<<<<<<< HEAD
#if 0
=======
>>>>>>> 2023/6/28
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

<<<<<<< HEAD
#define INPUT_SIZE 10
#define HIDDEN_SIZE 20
#define NUM_LAYERS 2
#define BATCH_SIZE 1
#define SEQ_LENGTH 5

// Define Matrix structure
typedef struct {
    float** data;
    int rows;
    int cols;
} Matrix;

// Create a new Matrix with specified rows and cols
Matrix create_matrix(int rows, int cols) {
    Matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.data = (float**)malloc(rows * sizeof(float*));
    for (int i = 0; i < rows; i++) {
        mat.data[i] = (float*)malloc(cols * sizeof(float));
    }
    return mat;
}

// Get the element at the specified row and col of the Matrix
float get_matrix_element(Matrix* mat, int row, int col) {
    return mat->data[row][col];
}

// Set the element at the specified row and col of the Matrix
void set_matrix_element(Matrix* mat, int row, int col, float value) {
    mat->data[row][col] = value;
}

// Free the memory allocated for the Matrix
void free_matrix(Matrix* mat) {
    for (int i = 0; i < mat->rows; i++) {
        free(mat->data[i]);
    }
    free(mat->data);
}

// Initialize LSTM model's weights and biases
void initialize_lstm(Matrix* weight_ih, Matrix* weight_hh, Matrix* bias_ih, Matrix* bias_hh) {
    // Initialize weight_ih
    for (int i = 0; i < weight_ih->rows; i++) {
        for (int j = 0; j < weight_ih->cols; j++) {
            set_matrix_element(weight_ih, i, j, 0.5);
        }
    }
    
    // Initialize weight_hh
    for (int i = 0; i < weight_hh->rows; i++) {
        for (int j = 0; j < weight_hh->cols; j++) {
            set_matrix_element(weight_hh, i, j, 0.3);
        }
    }
    
    // Initialize bias_ih and bias_hh
    for (int i = 0; i < bias_ih->rows; i++) {
        set_matrix_element(bias_ih, i, 0, 0.2);
        set_matrix_element(bias_hh, i, 0, 0.1);
    }
}

// Define sigmoid activation function
float sigmoid(float x) {
    return 1.0 / (1.0 + expf(-x));
}

// Define tanh activation function
float tanh_activation(float x) {
    return tanh(x);
}

// Perform one forward pass of LSTM
void lstm_forward(Matrix* weight_ih, Matrix* weight_hh, Matrix* bias_ih, Matrix* bias_hh,
                  Matrix* input_data, Matrix* hidden_state, Matrix* cell_state) {
    Matrix input_gate = create_matrix(HIDDEN_SIZE, 1);
    Matrix forget_gate = create_matrix(HIDDEN_SIZE, 1);
    Matrix cell_gate = create_matrix(HIDDEN_SIZE, 1);
    Matrix output_gate = create_matrix(HIDDEN_SIZE, 1);
    Matrix cell_state_next = create_matrix(HIDDEN_SIZE, 1);
    Matrix hidden_state_next = create_matrix(HIDDEN_SIZE, 1);
    
    // Calculate output and hidden state for the first time step
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        float weighted_sum_ih = 0.0;
        float weighted_sum_hh = 0.0;
        
        for (int j = 0; j < INPUT_SIZE; j++) {
            weighted_sum_ih += get_matrix_element(weight_ih, i, j) * get_matrix_element(input_data, j, 0);
        }
        
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            weighted_sum_hh += get_matrix_element(weight_hh, i, j) * get_matrix_element(hidden_state, j, 0);
        }
        
        float gates_sum = weighted_sum_ih + weighted_sum_hh +
                          get_matrix_element(bias_ih, i, 0) + get_matrix_element(bias_hh, i, 0);
        
        set_matrix_element(&input_gate, i, 0, sigmoid(gates_sum));
        set_matrix_element(&forget_gate, i, 0, sigmoid(gates_sum + HIDDEN_SIZE));
        set_matrix_element(&cell_gate, i, 0, tanh_activation(gates_sum + 2 * HIDDEN_SIZE));
        set_matrix_element(&output_gate, i, 0, sigmoid(gates_sum + 3 * HIDDEN_SIZE));
        
        set_matrix_element(&cell_state_next, i, 0, get_matrix_element(cell_state, i, 0) * get_matrix_element(&forget_gate, i, 0) +
                                                 get_matrix_element(&input_gate, i, 0) * get_matrix_element(&cell_gate, i, 0));
        set_matrix_element(&hidden_state_next, i, 0, get_matrix_element(&output_gate, i, 0) *
                                                     tanh_activation(get_matrix_element(&cell_state_next, i, 0)));
    }
    
    // Update hidden state and cell state
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        set_matrix_element(hidden_state, i, 0, get_matrix_element(&hidden_state_next, i, 0));
        set_matrix_element(cell_state, i, 0, get_matrix_element(&cell_state_next, i, 0));
    }
    
    // Free temporary matrices
    free_matrix(&input_gate);
    free_matrix(&forget_gate);
    free_matrix(&cell_gate);
    free_matrix(&output_gate);
    free_matrix(&cell_state_next);
    free_matrix(&hidden_state_next);
}

int main() {
    // Initialize LSTM model
    Matrix weight_ih = create_matrix(4 * HIDDEN_SIZE, INPUT_SIZE);
    Matrix weight_hh = create_matrix(4 * HIDDEN_SIZE, HIDDEN_SIZE);
    Matrix bias_ih = create_matrix(4 * HIDDEN_SIZE, 1);
    Matrix bias_hh = create_matrix(4 * HIDDEN_SIZE, 1);
    
    initialize_lstm(&weight_ih, &weight_hh, &bias_ih, &bias_hh);
    
    // Initialize hidden state and cell state
    Matrix hidden_state[NUM_LAYERS][BATCH_SIZE];
    Matrix cell_state[NUM_LAYERS][BATCH_SIZE];
    
    for (int i = 0; i < NUM_LAYERS; i++) {
        for (int j = 0; j < BATCH_SIZE; j++) {
            hidden_state[i][j] = create_matrix(HIDDEN_SIZE, 1);
            cell_state[i][j] = create_matrix(HIDDEN_SIZE, 1);
        }
    }
    
    // Create input data
    Matrix input_data[SEQ_LENGTH][BATCH_SIZE];
    
    for (int i = 0; i < SEQ_LENGTH; i++) {
        for (int j = 0; j < BATCH_SIZE; j++) {
            input_data[i][j] = create_matrix(INPUT_SIZE, 1);
            
            for (int k = 0; k < INPUT_SIZE; k++) {
                set_matrix_element(&input_data[i][j], k, 0, 0.4);
            }
        }
    }
    
    // Perform forward pass
    for (int i = 0; i < SEQ_LENGTH; i++) {
        lstm_forward(&weight_ih, &weight_hh, &bias_ih, &bias_hh,
                     &input_data[i][0], &hidden_state[0][0], &cell_state[0][0]);
    }
    
    // Output results
    printf("Output:\n");
    for (int i = 0; i < SEQ_LENGTH; i++) {
        for (int j = 0; j < BATCH_SIZE; j++) {
            for (int k = 0; k < HIDDEN_SIZE; k++) {
                printf("%f ", get_matrix_element(&hidden_state[0][j], k, 0));
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("\n");
    
    printf("Hidden State:\n");
    for (int i = 0; i < NUM_LAYERS; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            printf("%f ", get_matrix_element(&hidden_state[i][0], j, 0));
        }
        printf("\n");
    }
    
    printf("Cell State:\n");
    for (int i = 0; i < NUM_LAYERS; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            printf("%f ", get_matrix_element(&cell_state[i][0], j, 0));
        }
        printf("\n");
    }
    
    // Free memory
    free_matrix(&weight_ih);
    free_matrix(&weight_hh);
    free_matrix(&bias_ih);
    free_matrix(&bias_hh);
    
    for (int i = 0; i < NUM_LAYERS; i++) {
        for (int j = 0; j < BATCH_SIZE; j++) {
            free_matrix(&hidden_state[i][j]);
            free_matrix(&cell_state[i][j]);
        }
    }
    
    for (int i = 0; i < SEQ_LENGTH; i++) {
        for (int j = 0; j < BATCH_SIZE; j++) {
            free_matrix(&input_data[i][j]);
        }
    }
    
    return 0;
}
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define INPUT_SIZE 10
#define HIDDEN_SIZE 20
#define NUM_LAYERS 2
#define BATCH_SIZE 1
#define SEQ_LENGTH 5

// Matrix structure
typedef struct {
    int rows;
    int cols;
    float *data;
} Matrix;

// Create a matrix with specified rows and columns
Matrix create_matrix(int rows, int cols) {
    Matrix matrix;
    matrix.rows = rows;
    matrix.cols = cols;
    matrix.data = (float *)malloc(rows * cols * sizeof(float));
    return matrix;
}

// Get the value at the specified row and column of the matrix
float get_matrix_element(Matrix *matrix, int row, int col) {
    return matrix->data[row * matrix->cols + col];
}

// Set the value at the specified row and column of the matrix
void set_matrix_element(Matrix *matrix, int row, int col, float value) {
    matrix->data[row * matrix->cols + col] = value;
}

// Free the memory allocated for the matrix
void free_matrix(Matrix *matrix) {
    free(matrix->data);
}

// Initialize LSTM weights and biases
void initialize_lstm(Matrix *weight_ih, Matrix *weight_hh, Matrix *bias_ih, Matrix *bias_hh) {
    for (int i = 0; i < weight_ih->rows * weight_ih->cols; i++) {
        weight_ih->data[i] = 0.5;
    }
    
    for (int i = 0; i < weight_hh->rows * weight_hh->cols; i++) {
        weight_hh->data[i] = 0.3;
    }
    
    for (int i = 0; i < bias_ih->rows * bias_ih->cols; i++) {
        bias_ih->data[i] = 0.2;
    }
    
    for (int i = 0; i < bias_hh->rows * bias_hh->cols; i++) {
        bias_hh->data[i] = 0.1;
    }
}

// Perform one forward pass of the LSTM
void lstm_forward(Matrix *weight_ih, Matrix *weight_hh, Matrix *bias_ih, Matrix *bias_hh,
                  Matrix *input_data, Matrix *hidden_state, Matrix *cell_state) {
    int input_size = input_data->cols;
    int hidden_size = hidden_state->cols;
    
    Matrix input_gate = create_matrix(hidden_size, 1);
    Matrix forget_gate = create_matrix(hidden_size, 1);
    Matrix cell_gate = create_matrix(hidden_size, 1);
    Matrix output_gate = create_matrix(hidden_size, 1);
    
    Matrix cell_state_next = create_matrix(hidden_size, 1);
    Matrix hidden_state_next = create_matrix(hidden_size, 1);
    
    // Calculate the output and hidden state for the first time step
    for (int i = 0; i < hidden_size; i++) {
        float weighted_sum_ih = 0.0;
        float weighted_sum_hh = 0.0;
        
        for (int j = 0; j < input_size; j++) {
            weighted_sum_ih += weight_ih->data[i * input_size + j] * input_data->data[j];
        }
        
        for (int j = 0; j < hidden_size; j++) {
            weighted_sum_hh += weight_hh->data[i * hidden_size + j] * hidden_state->data[j];
        }
        
        float gates_sum = weighted_sum_ih + weighted_sum_hh + bias_ih->data[i] + bias_hh->data[i];
        
        input_gate.data[i] = 1.0 / (1.0 + expf(-gates_sum));
        forget_gate.data[i] = 1.0 / (1.0 + expf(-(gates_sum + hidden_size)));
        cell_gate.data[i] = tanh(gates_sum + 2 * hidden_size);
        output_gate.data[i] = 1.0 / (1.0 + expf(-(gates_sum + 3 * hidden_size)));
        
        cell_state_next.data[i] = forget_gate.data[i] * cell_state->data[0] + input_gate.data[i] * cell_gate.data[i];
        hidden_state_next.data[i] = output_gate.data[i] * tanh(cell_state_next.data[i]);
    }
    
    // Update the hidden state and cell state
    for (int i = 0; i < hidden_size; i++) {
        hidden_state->data[i] = hidden_state_next.data[i];
        cell_state->data[0] = cell_state_next.data[i];
    }
    
    // Free the allocated memory
    free_matrix(&input_gate);
    free_matrix(&forget_gate);
    free_matrix(&cell_gate);
    free_matrix(&output_gate);
    free_matrix(&cell_state_next);
    free_matrix(&hidden_state_next);
}

int main() {
    // Initialize LSTM weights and biases
    Matrix weight_ih = create_matrix(4 * HIDDEN_SIZE, INPUT_SIZE);
    Matrix weight_hh = create_matrix(4 * HIDDEN_SIZE, HIDDEN_SIZE);
    Matrix bias_ih = create_matrix(4 * HIDDEN_SIZE, 1);
    Matrix bias_hh = create_matrix(4 * HIDDEN_SIZE, 1);
    
    initialize_lstm(&weight_ih, &weight_hh, &bias_ih, &bias_hh);
    
    // Initialize hidden state and cell state
    Matrix hidden_state = create_matrix(NUM_LAYERS, BATCH_SIZE * HIDDEN_SIZE);
    Matrix cell_state = create_matrix(NUM_LAYERS, BATCH_SIZE * HIDDEN_SIZE);
    
    for (int i = 0; i < NUM_LAYERS * BATCH_SIZE * HIDDEN_SIZE; i++) {
        hidden_state.data[i] = 0.0;
        cell_state.data[i] = 0.0;
    }
    
    // Create input data
    Matrix input_data = create_matrix(SEQ_LENGTH, BATCH_SIZE * INPUT_SIZE);
    
    for (int i = 0; i < SEQ_LENGTH * BATCH_SIZE * INPUT_SIZE; i++) {
        input_data.data[i] = 0.4;
    }
    
    // Perform forward pass
    for (int i = 0; i < SEQ_LENGTH; i++) {
        lstm_forward(&weight_ih, &weight_hh, &bias_ih, &bias_hh,
                     &input_data, &hidden_state, &cell_state);
    }
#if 0
    
    // Output the result
    printf("Output:\n");
    for (int i = 0; i < NUM_LAYERS; i++) {
        for (int j = 0; j < BATCH_SIZE * HIDDEN_SIZE; j++) {
            printf("%f ", hidden_state.data[i * BATCH_SIZE * HIDDEN_SIZE + j]);
        }
        printf("\n");
    }

#endif
 
    // Output results
    printf("Output:\n");
    for (int i = 0; i < SEQ_LENGTH; i++) {
        for (int j = 0; j < BATCH_SIZE; j++) {
            for (int k = 0; k < HIDDEN_SIZE; k++) {
                printf("%f ", get_matrix_element(&hidden_state[0][j], k, 0));
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("\n");
    
    printf("Hidden State:\n");
    for (int i = 0; i < NUM_LAYERS; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            printf("%f ", get_matrix_element(&hidden_state[i][0], j, 0));
        }
        printf("\n");
    }
    
    printf("Cell State:\n");
    for (int i = 0; i < NUM_LAYERS; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            printf("%f ", get_matrix_element(&cell_state[i][0], j, 0));
        }
        printf("\n");
    }

    
    // Free the allocated memory
    free_matrix(&weight_ih);
    free_matrix(&weight_hh);
    free_matrix(&bias_ih);
    free_matrix(&bias_hh);
    free_matrix(&hidden_state);
    free_matrix(&cell_state);
    free_matrix(&input_data);
    
    return 0;
}

=======
#define FILTER_HEIGHT 3
#define FILTER_WIDTH 2
#define STRIDE_X 1
#define STRIDE_Y 2
#define PADDING 0
#define INPUT_HEIGHT 6
#define INPUT_WIDTH 8
#define INPUT_CHANNELS 3
#define OUTPUT_CHANNELS 4

float input_data[1][6][8][3] = {
    {
        {
            {0.1, 0.2, 0.3}, {0.4, 0.5, 0.6}, {0.7, 0.8, 0.9}, {1.0, 1.1, 1.2},
            {1.3, 1.4, 1.5}, {1.6, 1.7, 1.8}, {1.9, 2.0, 2.1}, {2.2, 2.3, 2.4}
        },
        {
            {2.5, 2.6, 2.7}, {2.8, 2.9, 3.0}, {3.1, 3.2, 3.3}, {3.4, 3.5, 3.6},
            {3.7, 3.8, 3.9}, {4.0, 4.1, 4.2}, {4.3, 4.4, 4.5}, {4.6, 4.7, 4.8}
        },
        {
            {4.9, 5.0, 5.1}, {5.2, 5.3, 5.4}, {5.5, 5.6, 5.7}, {5.8, 5.9, 6.0},
            {6.1, 6.2, 6.3}, {6.4, 6.5, 6.6}, {6.7, 6.8, 6.9}, {7.0, 7.1, 7.2}
        },
        {
            {7.3, 7.4, 7.5}, {7.6, 7.7, 7.8}, {7.9, 8.0, 8.1}, {8.2, 8.3, 8.4},
            {8.5, 8.6, 8.7}, {8.8, 8.9, 9.0}, {9.1, 9.2, 9.3}, {9.4, 9.5, 9.6}
        },
        {
            {9.7, 9.8, 9.9}, {10.0, 10.1, 10.2}, {10.3, 10.4, 10.5}, {10.6, 10.7, 10.8},
            {10.9, 11.0, 11.1}, {11.2, 11.3, 11.4}, {11.5, 11.6, 11.7}, {11.8, 11.9, 12.0}
        },
        {
            {12.1, 12.2, 12.3}, {12.4, 12.5, 12.6}, {12.7, 12.8, 12.9}, {13.0, 13.1, 13.2},
            {13.3, 13.4, 13.5}, {13.6, 13.7, 13.8}, {13.9, 14.0, 14.1}, {14.2, 14.3, 14.4}
        }
    }
};

float weights[1][3][2][3][4] = {
    {
        {
            {{0.1, 0.2, 0.3, 0.4}, {0.5, 0.6, 0.7, 0.8}, {0.9, 1.0, 1.1, 1.2}},
            {{1.3, 1.4, 1.5, 1.6}, {1.7, 1.8, 1.9, 2.0}, {2.1, 2.2, 2.3, 2.4}}
        },
		{
            {{0.1, 0.2, 0.3, 0.4}, {0.5, 0.6, 0.7, 0.8}, {0.9, 1.0, 1.1, 1.2}},
            {{1.3, 1.4, 1.5, 1.6}, {1.7, 1.8, 1.9, 2.0}, {2.1, 2.2, 2.3, 2.4}}
        },

        {
            {{2.5, 2.6, 2.7, 2.8}, {2.9, 3.0, 3.1, 3.2}, {3.3, 3.4, 3.5, 3.6}},
            {{3.7, 3.8, 3.9, 4.0}, {4.1, 4.2, 4.3, 4.4}, {4.5, 4.6, 4.7, 4.8}}
        }
    }
};

float bias[4] = {0.1, 0.2, 0.3, 0.4};
float output_data[1][3][7][4];
#if 0
void conv2D(float input_data[1][6][8][3], float weights[1][2][2][3][4], float bias[4], float output_data[1][3][7][4]) {
    int out_height = (INPUT_HEIGHT - FILTER_SIZE + 2 * PADDING) / STRIDE_Y + 1;
    int out_width = (INPUT_WIDTH - FILTER_SIZE + 2 * PADDING) / STRIDE_X + 1;



    for (int n = 0; n < 1; n++) {
        for (int h = 0; h < out_height; h++) {
            for (int w = 0; w < out_width; w++) {
                for (int oc = 0; oc < OUTPUT_CHANNELS; oc++) {
                    output_data[n][h][w][oc] = bias[oc];

                    for (int kh = 0; kh < FILTER_SIZE; kh++) {
                        for (int kw = 0; kw < FILTER_SIZE; kw++) {
                            for (int ic = 0; ic < INPUT_CHANNELS; ic++) {
                                int input_h = h * STRIDE_Y + kh - PADDING;
                                int input_w = w * STRIDE_X + kw - PADDING;

                                if (input_h >= 0 && input_h < INPUT_HEIGHT && input_w >= 0 && input_w < INPUT_WIDTH) {
                                    output_data[n][h][w][oc] += input_data[n][input_h][input_w][ic] * weights[0][kh][kw][ic][oc];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
#endif
void conv2D(float input_data[1 * 6 * 8 * 3], float weights[1 * 2 * 2 * 3 * 4], float bias[4], float output_data[1 * 3 * 7 * 4]) {
int out_height = (INPUT_HEIGHT - FILTER_HEIGHT + 2 * PADDING) / STRIDE_Y + 1;
int out_width = (INPUT_WIDTH - FILTER_WIDTH + 2 * PADDING) / STRIDE_X + 1;
int input_data_idx, weights_idx, output_data_idx;

for (int n = 0; n < 1; n++) {
    for (int h = 0; h < out_height; h++) {
        for (int w = 0; w < out_width; w++) {
            for (int oc = 0; oc < OUTPUT_CHANNELS; oc++) {
                output_data_idx = n * (out_height * out_width * OUTPUT_CHANNELS) + h * (out_width * OUTPUT_CHANNELS) + w * OUTPUT_CHANNELS + oc;
                output_data[output_data_idx] = bias[oc];

                for (int kh = 0; kh < FILTER_HEIGHT; kh++) {
                    for (int kw = 0; kw < FILTER_WIDTH; kw++) {
                        for (int ic = 0; ic < INPUT_CHANNELS; ic++) {
                            int input_h = h * STRIDE_Y + kh - PADDING;
                            int input_w = w * STRIDE_X + kw - PADDING;

                            if (input_h >= 0 && input_h < INPUT_HEIGHT && input_w >= 0 && input_w < INPUT_WIDTH) {
                                input_data_idx = n * (INPUT_HEIGHT * INPUT_WIDTH * INPUT_CHANNELS) + input_h * (INPUT_WIDTH * INPUT_CHANNELS) + input_w * INPUT_CHANNELS + ic;
                                weights_idx = kh * (FILTER_WIDTH * INPUT_CHANNELS * OUTPUT_CHANNELS) + kw * (INPUT_CHANNELS * OUTPUT_CHANNELS) + ic * OUTPUT_CHANNELS + oc;
                                output_data[output_data_idx] += input_data[input_data_idx] * weights[weights_idx];
                            }
                        }
                    }
                }
            }
        }
    }
}
}

int main() {
    // 执行Conv2D操作
    conv2D(input_data, weights, bias, output_data);

    // 打印输出结果
    for (int n = 0; n < 1; n++) {
        for (int h = 0; h < 3; h++) {
            for (int w = 0; w < 7; w++) {
                for (int oc = 0; oc < 4; oc++) {
                    printf("%f ", output_data[n][h][w][oc]);
                }
                printf("\n");
            }
            printf("\n");
        }
    }

    return 0;
}
#if 0
    float *input = input_data;
    float *weight = weights;
    float *output = output_data;
#endif
    //float bias
                    //output[n * (h * w * oc) + h * (w * oc) + w * oc + oc] = bias[oc];
                                    //output[n * (h * w * oc) + h * (w * oc) + w * oc + oc] +=
                                     //                                input[n * (input_h * input_w * ic) + input_h * (input_w * ic) + input_w * ic + ic] * weight[kh * (kw * (ic * oc)) + kw * (ic * oc) + ic * oc + oc];
>>>>>>> 2023/6/28

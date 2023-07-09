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
    float* data;
    int rows;
    int cols;
} Matrix;

// Create a new Matrix with specified rows and cols
Matrix create_matrix(int rows, int cols) {
    Matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.data = (float*)malloc(rows * cols * sizeof(float));
    return mat;
}

// Get the element at the specified row and col of the Matrix
float get_matrix_element(Matrix* mat, int row, int col) {
    return mat->data[row * mat->cols + col];
}

// Set the element at the specified row and col of the Matrix
void set_matrix_element(Matrix* mat, int row, int col, float value) {
    mat->data[row * mat->cols + col] = value;
}

// Free the memory allocated for the Matrix
void free_matrix(Matrix* mat) {
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

//matrix_multiply matrix_scalar_multiply
=======
// 定义BatchNormalization结构体
typedef struct {
    float gamma;
    float beta;
    float moving_mean;
    float moving_variance;
} BatchNormalization;

// 初始化BatchNormalization层
void initBatchNormalization(BatchNormalization* bn) {
    bn->gamma = 0.8;
    bn->beta = 0.2;
    bn->moving_mean = 1.0;
    bn->moving_variance = 0.5;
}

// 执行BatchNormalization
void batchNormalization(const float* input_data, int size, const BatchNormalization* bn, float* output) {
    for (int i = 0; i < size; i++) {
        float normalized_value = (input_data[i] - bn->moving_mean) / sqrtf(bn->moving_variance);
        output[i] = bn->gamma * normalized_value + bn->beta;
    }
}

int main() {
    // 创建输入张量
    int input_shape[] = {1, 1, 127, 4};
    int input_size = input_shape[0] * input_shape[1] * input_shape[2] * input_shape[3];
    float* input_data = (float*)malloc(input_size * sizeof(float));
    for (int i = 0; i < input_size; i++) {
        input_data[i] = 0.5;
    }

    // 创建BatchNormalization层并初始化权重
    BatchNormalization bn;
    initBatchNormalization(&bn);

    // 前向传播
    int output_size = input_size;
    float* output = (float*)malloc(output_size * sizeof(float));
    batchNormalization(input_data, output_size, &bn, output);

    // 打印输出结果
    printf("输入数据：\n");
    for (int i = 0; i < input_size; i++) {
        printf("%.2f ", input_data[i]);
    }
    printf("\n\nBatchNormalization层输出：\n");
    for (int i = 0; i < output_size; i++) {
        printf("%.2f ", output[i]);
    }
    printf("\n");

    // 释放内存
    free(input_data);
    free(output);

    return 0;
}

>>>>>>> 2023/6/28

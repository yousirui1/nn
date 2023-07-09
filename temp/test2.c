#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define INPUT_SIZE 10
#define HIDDEN_SIZE 20
#define NUM_LAYERS 2
#define BATCH_SIZE 1
#define SEQ_LENGTH 5

// 定义LSTM模型的结构体
typedef struct {
    float weight_ih[4 * HIDDEN_SIZE][INPUT_SIZE];
    float weight_hh[4 * HIDDEN_SIZE][HIDDEN_SIZE];
    float bias_ih[4 * HIDDEN_SIZE];
    float bias_hh[4 * HIDDEN_SIZE];
} LSTM;

// 初始化LSTM模型的权重和偏置
void initialize_lstm(LSTM* lstm) {
    for (int i = 0; i < 4 * HIDDEN_SIZE; i++) {
        for (int j = 0; j < INPUT_SIZE; j++) {
            lstm->weight_ih[i][j] = 0.5;
        }
    }
    
    for (int i = 0; i < 4 * HIDDEN_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            lstm->weight_hh[i][j] = 0.3;
        }
    }
    
    for (int i = 0; i < 4 * HIDDEN_SIZE; i++) {
        lstm->bias_ih[i] = 0.2;
        lstm->bias_hh[i] = 0.1;
    }
}

// 定义激活函数sigmoid
float sigmoid(float x) {
    return 1.0 / (1.0 + expf(-x));
}

// 定义激活函数tanh
float tanh_activation(float x) {
    return tanh(x);
}

// 执行一次LSTM的前向传播
void lstm_forward(LSTM* lstm, float* input_data, float* hidden_state, float* cell_state) {
    float input_gate[HIDDEN_SIZE];
    float forget_gate[HIDDEN_SIZE];
    float cell_gate[HIDDEN_SIZE];
    float output_gate[HIDDEN_SIZE];
    
    float cell_state_next[HIDDEN_SIZE];
    float hidden_state_next[HIDDEN_SIZE];
    
    // 计算第一个时间步的输出和隐藏状态
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        float weighted_sum_ih = 0.0;
        float weighted_sum_hh = 0.0;
        
        for (int j = 0; j < INPUT_SIZE; j++) {
            weighted_sum_ih += lstm->weight_ih[i][j] * input_data[j];
        }
        
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            weighted_sum_hh += lstm->weight_hh[i][j] * hidden_state[j];
        }
        
        float gates_sum = weighted_sum_ih + weighted_sum_hh + lstm->bias_ih[i] + lstm->bias_hh[i];
        
        input_gate[i] = sigmoid(gates_sum);
        forget_gate[i] = sigmoid(gates_sum + HIDDEN_SIZE);
        cell_gate[i] = tanh_activation(gates_sum + 2 * HIDDEN_SIZE);
        output_gate[i] = sigmoid(gates_sum + 3 * HIDDEN_SIZE);
        
        cell_state_next[i] = forget_gate[i] * (*cell_state) + input_gate[i] * cell_gate[i];
        hidden_state_next[i] = output_gate[i] * tanh_activation(cell_state_next[i]);
    }
    
    // 更新隐藏状态和细胞状态
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        hidden_state[i] = hidden_state_next[i];
        cell_state[i] = cell_state_next[i];
    }
}

int main() {
    LSTM lstm;
    initialize_lstm(&lstm);
    
    // 初始化隐藏状态和细胞状态
    float hidden_state[NUM_LAYERS][BATCH_SIZE][HIDDEN_SIZE];
    float cell_state[NUM_LAYERS][BATCH_SIZE][HIDDEN_SIZE];
    
    for (int i = 0; i < NUM_LAYERS; i++) {
        for (int j = 0; j < BATCH_SIZE; j++) {
            for (int k = 0; k < HIDDEN_SIZE; k++) {
                hidden_state[i][j][k] = 0.0;
                cell_state[i][j][k] = 0.0;
            }
        }
    }
    
    // 创建输入数据
    float input_data[SEQ_LENGTH][BATCH_SIZE][INPUT_SIZE];
    
    for (int i = 0; i < SEQ_LENGTH; i++) {
        for (int j = 0; j < BATCH_SIZE; j++) {
            for (int k = 0; k < INPUT_SIZE; k++) {
                input_data[i][j][k] = 0.4;
            }
        }
    }
    
    // 执行一次forward
    for (int i = 0; i < SEQ_LENGTH; i++) {
        lstm_forward(&lstm, input_data[i][0], hidden_state[0][0], cell_state[0][0]);
    }
    
    // 输出结果
#if 0
    printf("Output:\n");
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        printf("%f ", hidden_state[NUM_LAYERS - 1][0][i]);
    }
#endif

    printf("Output:\n");
    for (int i = 0; i < SEQ_LENGTH; i++) {
        for (int j = 0; j < BATCH_SIZE; j++) {
            for (int k = 0; k < HIDDEN_SIZE; k++) {
                printf("%f ", hidden_state[0][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }   
    printf("\n");
    
    printf("Hidden State:\n");
    for (int i = 0; i < NUM_LAYERS; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            printf("%f ", hidden_state[i][0][j]);
        }
        printf("\n");
    }
    
    printf("Cell State:\n");
    for (int i = 0; i < NUM_LAYERS; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            printf("%f ", cell_state[i][0][j]);
        }
        printf("\n");
    }
    
    return 0;
}


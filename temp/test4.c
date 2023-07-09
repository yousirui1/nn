#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define INPUT_SIZE 10
#define HIDDEN_SIZE 20
#define NUM_LAYERS 2
#define BATCH_SIZE 1
#define SEQ_LENGTH 5

typedef struct {
    float weight_ih[4 * HIDDEN_SIZE][INPUT_SIZE];
    float weight_hh[4 * HIDDEN_SIZE][HIDDEN_SIZE];
    float bias_ih[4 * HIDDEN_SIZE];
    float bias_hh[4 * HIDDEN_SIZE];
    float hidden[NUM_LAYERS][BATCH_SIZE][HIDDEN_SIZE];
} LSTM;

float sigmoid(float x) {
    return 1.0 / (1.0 + expf(-x));
}

float tanh_activation(float x) {
    return tanhf(x);
}

void initialize_lstm(LSTM* lstm) {
    int i, j;
    for (i = 0; i < 4 * HIDDEN_SIZE; i++) {
        for (j = 0; j < INPUT_SIZE; j++) {
            lstm->weight_ih[i][j] = 0.5;
        }
    }

    for (i = 0; i < 4 * HIDDEN_SIZE; i++) {
        for (j = 0; j < HIDDEN_SIZE; j++) {
            lstm->weight_hh[i][j] = 0.3;
        }
    }

    for (i = 0; i < 4 * HIDDEN_SIZE; i++) {
        lstm->bias_ih[i] = 0.2;
        lstm->bias_hh[i] = 0.1;
    }
}

void lstm_forward(LSTM* lstm, float input_data[SEQ_LENGTH][BATCH_SIZE][INPUT_SIZE]) {
    int t, i, j, k;
    float gates[4 * HIDDEN_SIZE];

    for (t = 0; t < SEQ_LENGTH; t++) {
        for (i = 0; i < 4 * HIDDEN_SIZE; i++) {
            gates[i] = lstm->bias_ih[i];
            for (j = 0; j < INPUT_SIZE; j++) {
                gates[i] += input_data[t][0][j] * lstm->weight_ih[i][j];
            }
            for (j = 0; j < HIDDEN_SIZE; j++) {
                gates[i] += lstm->hidden[0][0][j] * lstm->weight_hh[i][j];
            }
        }

        for (i = 0; i < 3 * HIDDEN_SIZE; i++) {
            gates[i] = tanh_activation(gates[i]);
        }
        for (i = 3 * HIDDEN_SIZE; i < 4 * HIDDEN_SIZE; i++) {
            gates[i] = sigmoid(gates[i]);
        }

        for (i = 0; i < HIDDEN_SIZE; i++) {
            lstm->hidden[0][0][i] = gates[HIDDEN_SIZE + i] * gates[i] +
                                    gates[2 * HIDDEN_SIZE + i] * lstm->hidden[0][0][i];
        }
    }
}

void print_output(LSTM* lstm) {
    int i, j, k;
    printf("Output:\n");
    for (i = 0; i < SEQ_LENGTH; i++) {
        for (j = 0; j < BATCH_SIZE; j++) {
            for (k = 0; k < HIDDEN_SIZE; k++) {
                printf("%f ", lstm->hidden[0][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }

    printf("Hidden state:\n");
    for (i = 0; i < NUM_LAYERS; i++) {
        for (j = 0; j < BATCH_SIZE; j++) {
            for (k = 0; k < HIDDEN_SIZE; k++) {
                printf("%f ", lstm->hidden[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

int main() {
    LSTM lstm;
    float input_data[SEQ_LENGTH][BATCH_SIZE][INPUT_SIZE];
    int i, j, k;

    initialize_lstm(&lstm);

    for (i = 0; i < SEQ_LENGTH; i++) {
        for (j = 0; j < BATCH_SIZE; j++) {
            for (k = 0; k < INPUT_SIZE; k++) {
                input_data[i][j][k] = 0.4;
            }
        }
    }

    lstm_forward(&lstm, input_data);

    print_output(&lstm);

    return 0;
}


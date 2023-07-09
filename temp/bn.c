#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BATCH_SIZE 1
#define INPUT_SIZE 3
#define HEIGHT 3
#define WIDTH 3

float input_data[BATCH_SIZE][INPUT_SIZE][HEIGHT][WIDTH] = {
    {
        //{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}},
        //{{10, 11, 12}, {13, 14, 15}, {16, 17, 18}},
        //{{19, 20, 21}, {22, 23, 24}, {25, 26, 27}}


         {{-1.1258, -1.1524, -0.2506},
          {-0.4339,  0.8487,  0.6920},
          {-0.3160, -2.1152,  0.3223}},

         {{-1.2633,  0.3500,  0.2660},
           {0.1665,  0.8744, -0.1435},
          {-0.1116, -0.6136,  0.0316}},

         {{-0.4927,  0.0537,  0.6181},
          {-0.4128, -0.8411, -2.3160},
          {-0.1023,  0.7924, -0.2897}}
    }
};

float weight[INPUT_SIZE] = {1, 1, 1};
float bias[INPUT_SIZE] = {0.0, 0.0, 0.0};

void batch_norm(float input_data[BATCH_SIZE][INPUT_SIZE][HEIGHT][WIDTH], float weight[INPUT_SIZE], float bias[INPUT_SIZE], float output_data[BATCH_SIZE][INPUT_SIZE][HEIGHT][WIDTH]) {
    float epsilon = 1e-5;
    float mean[INPUT_SIZE] = {0};
    float variance[INPUT_SIZE] = {0};
    
    // Compute mean
    for (int b = 0; b < BATCH_SIZE; b++) {
        for (int c = 0; c < INPUT_SIZE; c++) {
            for (int h = 0; h < HEIGHT; h++) {
                for (int w = 0; w < WIDTH; w++) {
                    mean[c] += input_data[b][c][h][w];
                }
            }
        }
    }
    
    for (int c = 0; c < INPUT_SIZE; c++) {
        mean[c] /= (BATCH_SIZE * HEIGHT * WIDTH);
    }
    
    // Compute variance
    for (int b = 0; b < BATCH_SIZE; b++) {
        for (int c = 0; c < INPUT_SIZE; c++) {
            for (int h = 0; h < HEIGHT; h++) {
                for (int w = 0; w < WIDTH; w++) {
                    float diff = input_data[b][c][h][w] - mean[c];
                    variance[c] += diff * diff;
                }
            }
        }
    }
    
    for (int c = 0; c < INPUT_SIZE; c++) {
        variance[c] /= (BATCH_SIZE * HEIGHT * WIDTH);
    }
    
    // Normalize and scale
    for (int b = 0; b < BATCH_SIZE; b++) {
        for (int c = 0; c < INPUT_SIZE; c++) {
            for (int h = 0; h < HEIGHT; h++) {
                for (int w = 0; w < WIDTH; w++) {
                    float normalized = (input_data[b][c][h][w] - mean[c]) / sqrt(variance[c] + epsilon);
                    output_data[b][c][h][w] = weight[c] * normalized + bias[c];
                }
            }
        }
    }
}

void print_output_data(float output_data[BATCH_SIZE][INPUT_SIZE][HEIGHT][WIDTH]) {
    for (int b = 0; b < BATCH_SIZE; b++) {
        for (int c = 0; c < INPUT_SIZE; c++) {
            for (int h = 0; h < HEIGHT; h++) {
                for (int w = 0; w < WIDTH; w++) {
                    printf("%.4f ", output_data[b][c][h][w]);
                }
                printf("\n");
            }
            printf("\n");
        }
        printf("\n");
    }
}

int main() {
    float output_data[BATCH_SIZE][INPUT_SIZE][HEIGHT][WIDTH];
    
    batch_norm(input_data, weight, bias, output_data);
    print_output_data(output_data);
    
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Structure to represent a matrix
typedef struct {
    int rows;
    int cols;
    float* data;
} Matrix;

// Function to create a matrix with the given dimensions
Matrix createMatrix(int rows, int cols) {
    Matrix matrix;
    matrix.rows = rows;
    matrix.cols = cols;
    matrix.data = (float*)malloc(rows * cols * sizeof(float));
    memset(matrix.data, 0, rows * cols * sizeof(float));
    return matrix;
}

// Function to free the memory allocated for a matrix
void freeMatrix(Matrix* matrix) {
    free(matrix->data);
    matrix->data = NULL;
}

// Function to get the value at the specified row and column of a matrix
float getMatrixValue(Matrix* matrix, int row, int col) {
    return matrix->data[row * matrix->cols + col];
}

// Function to set the value at the specified row and column of a matrix
void setMatrixValue(Matrix* matrix, int row, int col, float value) {
    matrix->data[row * matrix->cols + col] = value;
}

// Function to perform matrix multiplication: C = A * B
void matrixMultiply(Matrix* A, Matrix* B, Matrix* C) {
    int m = A->rows;
    int n = A->cols;
    int p = B->cols;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            float sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += getMatrixValue(A, i, k) * getMatrixValue(B, k, j);
            }
            setMatrixValue(C, i, j, sum);
        }
    }
}

// Define input dimensions
#define BATCH_SIZE 1
#define IN_CHANNELS 1
#define IN_HEIGHT 3
#define IN_WIDTH 3

// Define convolution transpose layer parameters
#define OUT_CHANNELS 2
#define KERNEL_SIZE 3
#define STRIDE 2
#define PADDING 1
#define OUTPUT_PADDING 1

// Define weight and bias for the convolution transpose layer
float weight[OUT_CHANNELS][IN_CHANNELS][KERNEL_SIZE][KERNEL_SIZE] = {
    {{{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}}},
    {{{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}}}
};

float bias[OUT_CHANNELS] = {0.0, 0.0};

// Define input tensor
float input_tensor[BATCH_SIZE][IN_CHANNELS][IN_HEIGHT][IN_WIDTH] = {
    {{{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}}}
};

// Function to perform the convolution transpose operation
void conv_transpose_2d(Matrix* input, Matrix* output, Matrix* weight, Matrix* bias) {
    int batch_size = input->rows;
    int input_channels = input->cols;
    int input_height = IN_HEIGHT;
    int input_width = IN_WIDTH;

    int output_channels = weight->rows / (input_channels * KERNEL_SIZE * KERNEL_SIZE);
    int kernel_size = KERNEL_SIZE;
    int stride = STRIDE;
    int padding = PADDING;
    int output_padding = OUTPUT_PADDING;

    int output_height = input_height * stride + padding * 2 - output_padding;
    int output_width = input_width * stride + padding * 2 - output_padding;

    // Create intermediate matrices
    Matrix input_reshaped = createMatrix(batch_size * input_height * input_width, input_channels);
    Matrix weight_reshaped = createMatrix(kernel_size * kernel_size * input_channels, output_channels);
    Matrix output_reshaped = createMatrix(batch_size * output_height * output_width, output_channels);

    // Reshape input tensor
    for (int b = 0; b < batch_size; b++) {
        for (int h = 0; h < input_height; h++) {
            for (int w = 0; w < input_width; w++) {
                for (int c = 0; c < input_channels; c++) {
                    int index = (b * input_height * input_width + h * input_width + w) * input_channels + c;
                    setMatrixValue(&input_reshaped, index, 0, getMatrixValue(input, b, c * input_height * input_width + h * input_width + w));
                }
            }
        }
    }

    // Reshape weight matrix
    for (int oc = 0; oc < output_channels; oc++) {
        for (int c = 0; c < input_channels; c++) {
            for (int kh = 0; kh < kernel_size; kh++) {
                for (int kw = 0; kw < kernel_size; kw++) {
                    int index = (oc * input_channels + c) * kernel_size * kernel_size + kh * kernel_size + kw;
                    setMatrixValue(&weight_reshaped, index, 0, getMatrixValue(weight, oc, c * kernel_size * kernel_size + kh * kernel_size + kw));
                }
            }
        }
    }

    // Perform matrix multiplication: output_reshaped = input_reshaped * weight_reshaped
    matrixMultiply(&input_reshaped, &weight_reshaped, &output_reshaped);

    // Reshape output tensor
    for (int b = 0; b < batch_size; b++) {
        for (int h = 0; h < output_height; h++) {
            for (int w = 0; w < output_width; w++) {
                for (int oc = 0; oc < output_channels; oc++) {
                    int index = (b * output_height * output_width + h * output_width + w) * output_channels + oc;
                    setMatrixValue(output, b, oc * output_height * output_width + h * output_width + w, getMatrixValue(&output_reshaped, index, 0) + getMatrixValue(bias, oc, 0));
                }
            }
        }
    }

    // Free the memory allocated for the intermediate matrices
    //freeMatrix(&input_reshaped);
    //freeMatrix(&weight_reshaped);
    //freeMatrix(&output_reshaped);
}

int main() {
    // Define output dimensions
    int output_height = IN_HEIGHT * STRIDE + PADDING * 2 - OUTPUT_PADDING;
    int output_width = IN_WIDTH * STRIDE + PADDING * 2 - OUTPUT_PADDING;

    // Create and initialize the output matrix
    Matrix output = createMatrix(BATCH_SIZE, OUT_CHANNELS * output_height * output_width);

    // Create input, weight, and bias matrices
    Matrix input = {BATCH_SIZE * IN_CHANNELS * IN_HEIGHT * IN_WIDTH, 1, (float*)input_tensor};
    Matrix weight_matrix = {OUT_CHANNELS * IN_CHANNELS * KERNEL_SIZE * KERNEL_SIZE, 1, (float*)weight};
    Matrix bias_matrix = {OUT_CHANNELS, 1, bias};

    // Perform the convolution transpose operation
    conv_transpose_2d(&input, &output, &weight_matrix, &bias_matrix);

    // Print the output shape and data
    printf("Output shape: %d x %d x %d x %d\n", BATCH_SIZE, OUT_CHANNELS, output_height, output_width);
    printf("Output data:\n");
    for (int b = 0; b < BATCH_SIZE; b++) {
        for (int oc = 0; oc < OUT_CHANNELS; oc++) {
            for (int h = 0; h < output_height; h++) {
                for (int w = 0; w < output_width; w++) {
                    printf("%.1f ", getMatrixValue(&output, b, oc * output_height * output_width + h * output_width + w));
                }
                printf("\n");
            }
            printf("\n");
        }
    }

    // Free the memory allocated for the output matrix
    freeMatrix(&output);

    return 0;
}


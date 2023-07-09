#include <stdio.h>
#include <stdlib.h>


// 定义输入大小
#define BATCH_SIZE 1
#define IN_CHANNELS 1
#define IN_HEIGHT 3
#define IN_WIDTH 3

// 定义卷积转置层参数
#define OUT_CHANNELS 2
#define KERNEL_SIZE 3
#define STRIDE 2
#define PADDING 1
#define OUTPUT_PADDING 1

// 定义卷积转置层的权重和偏置项
float weight[OUT_CHANNELS][IN_CHANNELS][KERNEL_SIZE][KERNEL_SIZE] = { 
    {{{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}}},
    {{{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}}}
};

float bias[OUT_CHANNELS] = {0.0, 0.0};

// 定义输入张量
float input_tensor[BATCH_SIZE][IN_CHANNELS][IN_HEIGHT][IN_WIDTH] = { 
    {{{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}}}
};

typedef struct {
    int rows;
    int cols;
    float *data;
} matrix;

matrix create_matrix(int rows, int cols) {
    matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.data = (float *)malloc(rows * cols * sizeof(float));
    return mat;
}

void free_matrix(matrix mat) {
    free(mat.data);
}

float get_element(matrix mat, int row, int col) {
    return mat.data[row * mat.cols + col];
}

void set_element(matrix mat, int row, int col, float value) {
    mat.data[row * mat.cols + col] = value;
}

matrix matrix_multiply(matrix mat1, matrix mat2) {
    if (mat1.cols != mat2.rows) {
        printf("Error: Incompatible matrix dimensions\n");
        exit(1);
    }
    
    matrix result = create_matrix(mat1.rows, mat2.cols);
    
    for (int i = 0; i < mat1.rows; i++) {
        for (int j = 0; j < mat2.cols; j++) {
            float sum = 0.0;
            for (int k = 0; k < mat1.cols; k++) {
                sum += get_element(mat1, i, k) * get_element(mat2, k, j);
            }
            set_element(result, i, j, sum);
        }
    }
    
    return result;
}

void conv_transpose_2d() {
    int batch, in_channel, out_channel, out_h, out_w, in_h, in_w, kernel_h, kernel_w;
    int stride_h = STRIDE;
    int stride_w = STRIDE;
    int padding_h = PADDING;
    int padding_w = PADDING;
    int output_padding_h = OUTPUT_PADDING;
    int output_padding_w = OUTPUT_PADDING;

    matrix input_tensor = create_matrix(BATCH_SIZE, IN_CHANNELS * IN_HEIGHT * IN_WIDTH);
    matrix weight = create_matrix(OUT_CHANNELS, IN_CHANNELS * KERNEL_SIZE * KERNEL_SIZE);
    matrix bias = create_matrix(OUT_CHANNELS, 1);
    matrix output = create_matrix(BATCH_SIZE, OUT_CHANNELS * ((IN_HEIGHT * stride_h + padding_h * 2 - OUTPUT_PADDING) * (IN_WIDTH * stride_w + padding_w * 2 - OUTPUT_PADDING)));
    
    // Initialize input_tensor, weight, bias, output with appropriate values
    
    for (batch = 0; batch < BATCH_SIZE; batch++) {
        for (out_channel = 0; out_channel < OUT_CHANNELS; out_channel++) {
            for (in_channel = 0; in_channel < IN_CHANNELS; in_channel++) {
                for (out_h = 0; out_h < IN_HEIGHT * stride_h + padding_h * 2 - OUTPUT_PADDING; out_h++) {
                    for (out_w = 0; out_w < IN_WIDTH * stride_w + padding_w * 2 - OUTPUT_PADDING; out_w++) {
                        matrix value = create_matrix(KERNEL_SIZE, KERNEL_SIZE);
                        for (kernel_h = 0; kernel_h < KERNEL_SIZE; kernel_h++) {
                            for (kernel_w = 0; kernel_w < KERNEL_SIZE; kernel_w++) {
                                in_h = (out_h - kernel_h + padding_h) / stride_h;
                                in_w = (out_w - kernel_w + padding_w) / stride_w;
                                if ((out_h - kernel_h + padding_h) % stride_h == 0 &&
                                    (out_w - kernel_w + padding_w) % stride_w == 0 &&
                                    in_h >= 0 && in_h < IN_HEIGHT && in_w >= 0 && in_w < IN_WIDTH) {
                                    set_element(value, kernel_h, kernel_w, get_element(input_tensor, batch, in_channel * IN_HEIGHT * IN_WIDTH + in_h * IN_WIDTH + in_w));
                                }
                            }
                        }
                        matrix result = matrix_multiply(value, weight);
                        float sum = 0.0;
                        for (int i = 0; i < result.rows; i++) {
                            for (int j = 0; j < result.cols; j++) {
                                sum += get_element(result, i, j);
                            }
                        }
                        set_element(output, batch, out_channel * ((IN_HEIGHT * stride_h + padding_h * 2 - OUTPUT_PADDING) * (IN_WIDTH * stride_w + padding_w * 2 - OUTPUT_PADDING)) + out_h * (IN_WIDTH * stride_w + padding_w * 2 - OUTPUT_PADDING) + out_w, sum + get_element(bias, out_channel, 0));
                        free_matrix(value);
                        free_matrix(result);
                    }
                }
            }
        }
    }

    // Use output matrix as needed
    
    free_matrix(input_tensor);
    free_matrix(weight);
    free_matrix(bias);
    free_matrix(output);
}

int main() {
    conv_transpose_2d();
    return 0;
}



#include <stdio.h>
#include <stdlib.h>

#define BATCH_SIZE 2
#define IN_CHANNELS 3
#define OUT_CHANNELS 4
#define IN_HEIGHT 5
#define IN_WIDTH 5
#define KERNEL_SIZE 3
#define STRIDE 1
#define PADDING 1
#define OUTPUT_PADDING 0

typedef struct {
    int rows;
    int cols;
    float *data;
} matrix;

matrix create_matrix(int rows, int cols) {
    matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.data = (float *)malloc(rows * cols * sizeof(float));
    return mat;
}

void free_matrix(matrix mat) {
    free(mat.data);
}

float get_element(matrix mat, int row, int col) {
    return mat.data[row * mat.cols + col];
}

void set_element(matrix mat, int row, int col, float value) {
    mat.data[row * mat.cols + col] = value;
}

matrix matrix_multiply(matrix mat1, matrix mat2) {
    if (mat1.cols != mat2.rows) {
        printf("Error: Incompatible matrix dimensions\n");
        exit(1);
    }
    
    matrix result = create_matrix(mat1.rows, mat2.cols);
    
    for (int i = 0; i < mat1.rows; i++) {
        for (int j = 0; j < mat2.cols; j++) {
            float sum = 0.0;
            for (int k = 0; k < mat1.cols; k++) {
                sum += get_element(mat1, i, k) * get_element(mat2, k, j);
            }
            set_element(result, i, j, sum);
        }
    }
    
    return result;
}

void conv_transpose_2d() {
    int batch, in_channel, out_channel, out_h, out_w, in_h, in_w, kernel_h, kernel_w;
    int stride_h = STRIDE;
    int stride_w = STRIDE;
    int padding_h = PADDING;
    int padding_w = PADDING;
    int output_padding_h = OUTPUT_PADDING;
    int output_padding_w = OUTPUT_PADDING;

    matrix input_tensor = create_matrix(BATCH_SIZE, IN_CHANNELS * IN_HEIGHT * IN_WIDTH);
    matrix weight = create_matrix(OUT_CHANNELS, IN_CHANNELS * KERNEL_SIZE * KERNEL_SIZE);
    matrix bias = create_matrix(OUT_CHANNELS, 1);
    matrix output = create_matrix(BATCH_SIZE, OUT_CHANNELS * ((IN_HEIGHT * stride_h + padding_h * 2 - OUTPUT_PADDING) * (IN_WIDTH * stride_w + padding_w * 2 - OUTPUT_PADDING)));
    
    // Initialize input_tensor, weight, bias, output with appropriate values
    
    for (batch = 0; batch < BATCH_SIZE; batch++) {
        for (out_channel = 0; out_channel < OUT_CHANNELS; out_channel++) {
            for (in_channel = 0; in_channel < IN_CHANNELS; in_channel++) {
                for (out_h = 0; out_h < IN_HEIGHT * stride_h + padding_h * 2 - OUTPUT_PADDING; out_h++) {
                    for (out_w = 0; out_w < IN_WIDTH * stride_w + padding_w * 2 - OUTPUT_PADDING; out_w++) {
                        matrix value = create_matrix(KERNEL_SIZE, KERNEL_SIZE);
                        for (kernel_h = 0; kernel_h < KERNEL_SIZE; kernel_h++) {
                            for (kernel_w = 0; kernel_w < KERNEL_SIZE; kernel_w++) {
                                in_h = (out_h - kernel_h + padding_h) / stride_h;
                                in_w = (out_w - kernel_w + padding_w) / stride_w;
                                if ((out_h - kernel_h + padding_h) % stride_h == 0 &&
                                    (out_w - kernel_w + padding_w) % stride_w == 0 &&
                                    in_h >= 0 && in_h < IN_HEIGHT && in_w >= 0 && in_w < IN_WIDTH) {
                                    set_element(value, kernel_h, kernel_w, get_element(input_tensor, batch, in_channel * IN_HEIGHT * IN_WIDTH + in_h * IN_WIDTH + in_w));
                                }
                            }
                        }
                        matrix result = matrix_multiply(value, weight);
                        float sum = 0.0;
                        for (int i = 0; i < result.rows; i++) {
                            for (int j = 0; j < result.cols; j++) {
                                sum += get_element(result, i, j);
                            }
                        }
                        set_element(output, batch, out_channel * ((IN_HEIGHT * stride_h + padding_h * 2 - OUTPUT_PADDING) * (IN_WIDTH * stride_w + padding_w * 2 - OUTPUT_PADDING)) + out_h * (IN_WIDTH * stride_w + padding_w * 2 - OUTPUT_PADDING) + out_w, sum + get_element(bias, out_channel, 0));
                        free_matrix(value);
                        free_matrix(result);
                    }
                }
            }
        }
    }

    // Use output matrix as needed
    
    free_matrix(input_tensor);
    free_matrix(weight);
    free_matrix(bias);
    free_matrix(output);
}

int main() {
    conv_transpose_2d();
    return 0;
}


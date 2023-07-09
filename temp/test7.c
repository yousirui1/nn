#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

// 定义Matrix结构体
typedef struct {
    int rows;
    int cols;
    float *data;
} Matrix;

// 创建Matrix结构体
Matrix createMatrix(int rows, int cols) {
    Matrix matrix;
    matrix.rows = rows;
    matrix.cols = cols;
    matrix.data = (float *)malloc(rows * cols * sizeof(float));
    return matrix;
}

// 释放Matrix结构体内存
void freeMatrix(Matrix *matrix) {
    free(matrix->data);
}

// 获取Matrix的元素值
float getMatrixValue(Matrix *matrix, int row, int col) {
    return matrix->data[row * matrix->cols + col];
}

// 设置Matrix的元素值
void setMatrixValue(Matrix *matrix, int row, int col, float value) {
    matrix->data[row * matrix->cols + col] = value;
}

// 定义卷积转置层的权重和偏置项
Matrix weight = {
    OUT_CHANNELS * IN_CHANNELS * KERNEL_SIZE * KERNEL_SIZE,
    1,
    (float []) {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}
};

Matrix bias = {
    OUT_CHANNELS,
    1,
    (float []) {0.0, 0.0}
};

// 定义输入张量
Matrix input_tensor = {
    BATCH_SIZE,
    IN_CHANNELS * IN_HEIGHT * IN_WIDTH,
    (float []) {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}
};

// 定义输出张量
Matrix output = {
    BATCH_SIZE,
    OUT_CHANNELS * (IN_HEIGHT * STRIDE + PADDING * 2 - OUTPUT_PADDING) * (IN_WIDTH * STRIDE + PADDING * 2 - OUTPUT_PADDING),
    NULL
};

#if 1
// 卷积转置运算
void conv_transpose_2d() {
    int batch, out_channel, out_h, out_w, in_h, in_w, kernel_h, kernel_w;
    int stride_h = STRIDE;
    int stride_w = STRIDE;
    int padding_h = PADDING;
    int padding_w = PADDING;
    int output_padding_h = OUTPUT_PADDING;
    int output_padding_w = OUTPUT_PADDING;

    output.data = (float *)malloc(output.rows * output.cols * sizeof(float));

    printf("batch_size %d out_channels %d col_h %d col_w %d kernel_h %d kernel_w %d \n ",
            BATCH_SIZE, OUT_CHANNELS, IN_HEIGHT * stride_h + padding_h * 2 - OUTPUT_PADDING, IN_WIDTH * stride_w + padding_w * 2 - OUTPUT_PADDING, KERNEL_SIZE, KERNEL_SIZE);

    printf("IN_HEIGHT %d IN_HEIGHT %d STRIDE %d OUTPUT_PADDING %d size %d ",
           IN_HEIGHT, IN_WIDTH, STRIDE, OUTPUT_PADDING, OUT_CHANNELS * (IN_HEIGHT * STRIDE + PADDING * 2 - OUTPUT_PADDING) * (IN_WIDTH * STRIDE + PADDING * 2 - OUTPUT_PADDING));

    for (batch = 0; batch < BATCH_SIZE; batch++) {
        for (out_channel = 0; out_channel < OUT_CHANNELS; out_channel++) {
            for (out_h = 0; out_h < IN_HEIGHT * stride_h + padding_h * 2 - OUTPUT_PADDING; out_h++) {
                for (out_w = 0; out_w < IN_WIDTH * stride_w + padding_w * 2 - OUTPUT_PADDING; out_w++) {
                    float value = 0.0;
                    for (kernel_h = 0; kernel_h < KERNEL_SIZE; kernel_h++) {
                        for (kernel_w = 0; kernel_w < KERNEL_SIZE; kernel_w++) {
                            in_h = (out_h - kernel_h + padding_h) / stride_h;
                            in_w = (out_w - kernel_w + padding_w) / stride_w;
                            if ((out_h - kernel_h + padding_h) % stride_h == 0 &&
                                (out_w - kernel_w + padding_w) % stride_w == 0 &&
                                in_h >= 0 && in_h < IN_HEIGHT && in_w >= 0 && in_w < IN_WIDTH) {
                                value += getMatrixValue(&input_tensor, batch, in_h * IN_WIDTH + in_w) * getMatrixValue(&weight, out_channel * IN_CHANNELS * KERNEL_SIZE * KERNEL_SIZE + in_h * KERNEL_SIZE + kernel_w, 0);
                                //printf("value %f \n", value) ;
                                //printf("value %f \n", getMatrixValue(&weight, out_channel * IN_CHANNELS * KERNEL_SIZE * KERNEL_SIZE + in_h * KERNEL_SIZE + kernel_w, 0)) ;
                                printf("value %d \n", out_channel * IN_CHANNELS * KERNEL_SIZE * KERNEL_SIZE + in_h * KERNEL_SIZE + kernel_w) ;
                                //printf("value %f \n", value);
                            }
                        }
                    }
                    setMatrixValue(&output, batch, out_channel * (IN_HEIGHT * STRIDE + PADDING * 2 - OUTPUT_PADDING) * (IN_WIDTH * STRIDE + PADDING * 2 - OUTPUT_PADDING) + out_h * (IN_WIDTH * STRIDE + PADDING * 2 - OUTPUT_PADDING) + out_w, value + getMatrixValue(&bias, out_channel, 0));
                }
            }
        }
    }
}
#endif
#if 0
void conv_transpose_2d() {
    int batch, out_channel, out_h, out_w, in_h, in_w, kernel_h, kernel_w;
    int stride_h = STRIDE;
    int stride_w = STRIDE;
    int padding_h = PADDING;
    int padding_w = PADDING;
    int output_padding_h = OUTPUT_PADDING;
    int output_padding_w = OUTPUT_PADDING;
    int output_rows = output.rows;
    int output_cols = output.cols;

    output.data = (float *)malloc(output_rows * output_cols * sizeof(float));

    int in_height_stride = IN_HEIGHT * stride_h + padding_h * 2 - OUTPUT_PADDING;
    int in_width_stride = IN_WIDTH * stride_w + padding_w * 2 - OUTPUT_PADDING;
    int kernel_size_square = KERNEL_SIZE * KERNEL_SIZE;

    for (batch = 0; batch < BATCH_SIZE; batch++) {
        for (out_channel = 0; out_channel < OUT_CHANNELS; out_channel++) {
            int out_channel_offset = out_channel * (in_height_stride) * (in_width_stride);
            int out_channel_bias = out_channel * output_rows * output_cols;

            for (out_h = 0; out_h < in_height_stride; out_h++) {
                int out_h_offset = out_h * (in_width_stride);
                int in_h_start = (out_h - padding_h) / stride_h;
                int in_h_end = (out_h - padding_h + KERNEL_SIZE) / stride_h;

                for (out_w = 0; out_w < in_width_stride; out_w++) {
                    int out_w_offset = out_w;
                    int in_w_start = (out_w - padding_w) / stride_w;
                    int in_w_end = (out_w - padding_w + KERNEL_SIZE) / stride_w;

                    float value = 0.0;

                    for (kernel_h = 0; kernel_h < KERNEL_SIZE; kernel_h++) {
                        int in_h = in_h_start + kernel_h;
                        int kernel_h_offset = kernel_h * KERNEL_SIZE;

                        if (in_h >= 0 && in_h < IN_HEIGHT) {
                            for (kernel_w = 0; kernel_w < KERNEL_SIZE; kernel_w++) {
                                int in_w = in_w_start + kernel_w;
                                int kernel_w_offset = kernel_w;

                                if (in_w >= 0 && in_w < IN_WIDTH) {
                                    int weight_offset = out_channel_offset +
                                        in_h * KERNEL_SIZE * IN_CHANNELS +
                                        kernel_h_offset +
                                        in_w * IN_CHANNELS +
                                        kernel_w_offset;

                                    int input_offset = batch * IN_HEIGHT * IN_WIDTH + in_h * IN_WIDTH + in_w;

                                    value += input_tensor.data[input_offset] * weight.data[weight_offset];
                                }
                            }
                        }
                    }

                    int output_offset = batch * output_rows * output_cols +
                        out_channel_bias +
                        out_h_offset +
                        out_w_offset;

                    output.data[output_offset] = value + bias.data[out_channel];
                }
            }
        }
    }
}
#endif


int main() {
    // 进行卷积转置运算
    //
    //
     for(int i = 0; i < weight.rows * weight.cols; i++)
    {
        printf("index %d : %f \n",i , weight.data[i]);
    }

    printf("%f ", getMatrixValue(&weight, 17, 0));

    conv_transpose_2d();
    printf("OUT_CHANNELS * IN_CHANNELS * KERNEL_SIZE * KERNEL_SIZE, %d \n", OUT_CHANNELS * IN_CHANNELS * KERNEL_SIZE * KERNEL_SIZE);


       // 打印输出的形状和结果
    printf("Output shape: %d x %d x %d x %d\n", output.rows, OUT_CHANNELS, IN_HEIGHT * STRIDE + PADDING * 2 - OUTPUT_PADDING, IN_WIDTH * STRIDE + PADDING * 2 - OUTPUT_PADDING);
    printf("Output data:\n");
    for (int i = 0; i < output.rows; i++) {
        for (int j = 0; j < OUT_CHANNELS; j++) {
            for (int k = 0; k < IN_HEIGHT * STRIDE + PADDING * 2 - OUTPUT_PADDING; k++) {
                for (int l = 0; l < IN_WIDTH * STRIDE + PADDING * 2 - OUTPUT_PADDING; l++) {
                    printf("%.1f ", getMatrixValue(&output, i, j * (IN_HEIGHT * STRIDE + PADDING * 2 - OUTPUT_PADDING) * (IN_WIDTH * STRIDE + PADDING * 2 - OUTPUT_PADDING) + k * (IN_WIDTH * STRIDE + PADDING * 2 - OUTPUT_PADDING) + l));
                }
                printf("\n");
            }
            printf("\n");
        }
    }

    

    // 释放内存
    freeMatrix(&input_tensor);
    freeMatrix(&output);

    return 0;
}


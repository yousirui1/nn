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

// 定义输出张量
float output[BATCH_SIZE][OUT_CHANNELS][IN_HEIGHT * STRIDE + PADDING * 2 - OUTPUT_PADDING][IN_WIDTH * STRIDE + PADDING * 2 - OUTPUT_PADDING];

// 卷积转置运算
void conv_transpose_2d() {
    int batch, in_channel, out_channel, out_h, out_w, in_h, in_w, kernel_h, kernel_w;
    int stride_h = STRIDE;
    int stride_w = STRIDE;
    int padding_h = PADDING;
    int padding_w = PADDING;
    int output_padding_h = OUTPUT_PADDING;
    int output_padding_w = OUTPUT_PADDING;

    for (batch = 0; batch < BATCH_SIZE; batch++) {
        for (out_channel = 0; out_channel < OUT_CHANNELS; out_channel++) {
            for (in_channel = 0; in_channel < IN_CHANNELS; in_channel++) {
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
                                    value += input_tensor[batch][in_channel][in_h][in_w] * weight[out_channel][in_channel][kernel_h][kernel_w];
                                }
                            }
                        }
                        output[batch][out_channel][out_h][out_w] = value + bias[out_channel];
                    }
                }
            }
        }
    }
}

int main() {
    // 进行卷积转置运算
    conv_transpose_2d();

    // 打印输出的形状和结果
    printf("Output shape: %d x %d x %d x %d\n", BATCH_SIZE, OUT_CHANNELS, IN_HEIGHT * STRIDE + PADDING * 2 - OUTPUT_PADDING, IN_WIDTH * STRIDE + PADDING * 2 - OUTPUT_PADDING);
    printf("Output data:\n");
    for (int i = 0; i < BATCH_SIZE; i++) {
        for (int j = 0; j < OUT_CHANNELS; j++) {
            for (int k = 0; k < IN_HEIGHT * STRIDE + PADDING * 2 - OUTPUT_PADDING; k++) {
                for (int l = 0; l < IN_WIDTH * STRIDE + PADDING * 2 - OUTPUT_PADDING; l++) {
                    printf("%.1f ", output[i][j][k][l]);
                }
                printf("\n");
            }
            printf("\n");
        }
    }

    return 0;
}


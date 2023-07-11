#include "base.h"

void add_biases(float* out, float* biases, int out_c, int out_cols) {
    for (int i = 0; i < out_c; i++) 
    {   
        float bias = biases[i];
        for (int j = 0; j < out_cols; j++) 
        {
            out[i * out_cols + j] += bias;
        }
    }   
}

void transpose_weights(float *weights, int channels, int size_h, int size_w)
{
    float *tmp = (float *)calloc(size_h * size_w * channels, sizeof(float));
    for (int i = 0; i < size_h; ++i) {
        for (int j = 0; j < size_w; ++j) {
            for (int k = 0; k < channels; ++k) {
                int dst_index = i * size_w * channels + j * channels + k;
                int src_index = k * size_h * size_w + i * size_w + j;
                tmp[dst_index] = weights[src_index];
            }
        }
    }   
    memcpy(weights, tmp, size_h * size_w * channels * sizeof(float));
    free(tmp);
}


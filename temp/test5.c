#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    int rows;
    int cols;
    float* data;
} Matrix;

float get_element(const Matrix* mat, int row, int col) {
    return mat->data[row * mat->cols + col];
}

void set_element(Matrix* mat, int row, int col, float value) {
    mat->data[row * mat->cols + col] = value;
}

Matrix* create_matrix(int rows, int cols) {
    Matrix* mat = (Matrix*)malloc(sizeof(Matrix));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = (float*)malloc(rows * cols * sizeof(float));
    return mat;
}

Matrix* matrix_multiply(const Matrix* mat1, const Matrix* mat2) {
    int m = mat1->rows;
    int n = mat1->cols;
    int p = mat2->cols;
    Matrix* result = create_matrix(m, p);

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            float sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += get_element(mat1, i, k) * get_element(mat2, k, j);
            }
            set_element(result, i, j, sum);
        }
    }

    return result;
}

void batch_norm(Matrix* input_data, Matrix* weight, Matrix* bias, Matrix* output_data) {
    float epsilon = 1e-5;
    int input_size = input_data->cols;
    int height = input_data->rows;
    int width = 1;

    Matrix* mean = create_matrix(input_size, width);
    Matrix* variance = create_matrix(input_size, width);

    // Compute mean
    for (int c = 0; c < input_size; c++) {
        float sum = 0.0;
        for (int h = 0; h < height; h++) {
            sum += get_element(input_data, h, c);
        }
        set_element(mean, c, 0, sum / height);
    }

    // Compute variance
    for (int c = 0; c < input_size; c++) {
        float sum = 0.0;
        for (int h = 0; h < height; h++) {
            float diff = get_element(input_data, h, c) - get_element(mean, c, 0);
            sum += diff * diff;
        }
        set_element(variance, c, 0, sum / height);
    }

    // Normalize and scale
    for (int c = 0; c < input_size; c++) {
        float mean_value = get_element(mean, c, 0);
        float variance_value = get_element(variance, c, 0);
        for (int h = 0; h < height; h++) {
            float normalized = (get_element(input_data, h, c) - mean_value) / sqrtf(variance_value + epsilon);
            float scaled = get_element(weight, c, 0) * normalized + get_element(bias, c, 0);
            set_element(output_data, h, c, scaled);
        }
    }

    // Cleanup
    free(mean->data);
    free(mean);
    free(variance->data);
    free(variance);
}

void print_output_data(Matrix* output_data) {
    int input_size = output_data->cols;
    int height = output_data->rows;

    for (int h = 0; h < height; h++) {
        for (int c = 0; c < input_size; c++) {
            printf("%.4f ", get_element(output_data, h, c));
        }
        printf("\n");
    }
}

int main() {
    Matrix* input_data = create_matrix(3, 3);
    set_element(input_data, 0, 0, -1.1258);
    set_element(input_data, 0, 1, -1.1524);
    set_element(input_data, 0, 2, -0.2506);
    set_element(input_data, 1, 0, -0.4339);
    set_element(input_data, 1, 1, 0.8487);
    set_element(input_data, 1, 2, 0.6920);
    set_element(input_data, 2, 0, -0.3160);
    set_element(input_data, 2, 1, -2.1152);
    set_element(input_data, 2, 2, 0.3223);

    Matrix* weight = create_matrix(3, 1);
    set_element(weight, 0, 0, 1);
    set_element(weight, 1, 0, 1);
    set_element(weight, 2, 0, 1);

    Matrix* bias = create_matrix(3, 1);
    set_element(bias, 0, 0, 0.0);
    set_element(bias, 1, 0, 0.0);
    set_element(bias, 2, 0, 0.0);

    Matrix* output_data = create_matrix(3, 3);

    batch_norm(input_data, weight, bias, output_data);
    print_output_data(output_data);

    // Cleanup
    free(input_data->data);
    free(input_data);
    free(weight->data);
    free(weight);
    free(bias->data);
    free(bias);
    free(output_data->data);
    free(output_data);

    return 0;
}


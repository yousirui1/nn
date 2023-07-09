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

typedef struct {
    int batch_size;
    int input_size;
    int height;
    int width;
    Matrix* data;
} Tensor;

Tensor* create_tensor(int batch_size, int input_size, int height, int width) {
    Tensor* tensor = (Tensor*)malloc(sizeof(Tensor));
    tensor->batch_size = batch_size;
    tensor->input_size = input_size;
    tensor->height = height;
    tensor->width = width;
    tensor->data = (Matrix*)malloc(batch_size * input_size * sizeof(Matrix));
    
    for (int b = 0; b < batch_size; b++) {
        for (int c = 0; c < input_size; c++) {
            tensor->data[b * input_size + c] = *create_matrix(height, width);
        }
    }
    
    return tensor;
}

float get_tensor_element(const Tensor* tensor, int batch, int input, int row, int col) {
    return get_element(&(tensor->data[batch * tensor->input_size + input]), row, col);
}

void set_tensor_element(Tensor* tensor, int batch, int input, int row, int col, float value) {
    set_element(&(tensor->data[batch * tensor->input_size + input]), row, col, value);
}

void batch_norm(Tensor* input_tensor, float weight[], float bias[], Tensor* output_tensor) {
    int batch_size = input_tensor->batch_size;
    int input_size = input_tensor->input_size;
    int height = input_tensor->height;
    int width = input_tensor->width;
    float epsilon = 1e-5;
    
    for (int b = 0; b < batch_size; b++) {
        for (int c = 0; c < input_size; c++) {
            for (int h = 0; h < height; h++) {
                for (int w = 0; w < width; w++) {
                    float mean = 0.0;
                    float variance = 0.0;
                    
                    // Compute mean
                    for (int i = 0; i < height; i++) {
                        for (int j = 0; j < width; j++) {
                            mean += get_tensor_element(input_tensor, b, c, i, j);
                        }
                    }
                    mean /= (height * width);
                    
                    // Compute variance
                    for (int i = 0; i < height; i++) {
                        for (int j = 0; j < width; j++) {
                            float diff = get_tensor_element(input_tensor, b, c, i, j) - mean;
                            variance += diff * diff;
                        }
                    }
                    variance /= (height * width);
                    
                    // Normalize and scale
                    float normalized = (get_tensor_element(input_tensor, b, c, h, w) - mean) / sqrt(variance + epsilon);
                    float output = weight[c] * normalized + bias[c];
                    set_tensor_element(output_tensor, b, c, h, w, output);
                }
            }
        }
    }
}

void print_tensor_data(Tensor* tensor) {
    int batch_size = tensor->batch_size;
    int input_size = tensor->input_size;
    int height = tensor->height;
    int width = tensor->width;
    
    for (int b = 0; b < batch_size; b++) {
        printf("Batch %d:\n", b);
        for (int c = 0; c < input_size; c++) {
            printf("Input %d:\n", c);
            for (int h = 0; h < height; h++) {
                for (int w = 0; w < width; w++) {
                    printf("%.4f ", get_tensor_element(tensor, b, c, h, w));
                }
                printf("\n");
            }
            printf("\n");
        }
        printf("\n");
    }
}

int main() {
    int batch_size = 1;
    int input_size = 3;
    int height = 3;
    int width = 3;
    
    Tensor* input_tensor = create_tensor(batch_size, input_size, height, width);
    Tensor* output_tensor = create_tensor(batch_size, input_size, height, width);
    
    // Initialize input_tensor with input_data
    for (int b = 0; b < batch_size; b++) {
        for (int c = 0; c < input_size; c++) {
            for (int h = 0; h < height; h++) {
                for (int w = 0; w < width; w++) {
                    set_tensor_element(input_tensor, b, c, h, w, input_data[b][c][h][w]);
                }
            }
        }
    }
    
    batch_norm(input_tensor, weight, bias, output_tensor);
    print_tensor_data(output_tensor);
    
    return 0;
}


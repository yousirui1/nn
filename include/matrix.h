#ifndef __MATRIX_H__
#define __MATRIX_H__

#define MAX_DIMS 8

struct shape_t 
{
    int num_dims;
    int size;
    int dims[MAX_DIMS];
};

typedef struct shape_t shape_t;


struct matrix_t{
    //int rows; 
    //int cols;
	shape_t shape;
    float *data;
};

typedef struct matrix_t matrix_t;

matrix_t *matrix_alloc(int num_dims, ...);
matrix_t *matrix_alloc_shape(shape_t shape);
matrix_t *matrix_empty(int num_dims, ...);
matrix_t *matrix_shape_empty(shape_t shape);
matrix_t *matrix_empty_shape(shape_t shape);

int matrix_set(matrix_t *mat, float value, int num_dims, ...);
float matrix_get(matrix_t *mat, int num_dims, ...);
void matrix_free(matrix_t *mat);

matrix_t *matrix_copy(matrix_t *src);
int matrix_apply_sum(matrix_t *dst, matrix_t *src,  float scalar);
matrix_t* matrix_sum(matrix_t *dst, matrix_t *src,  float scalar);
//matrix_t *matrix_multiply(matrix_t *a, matrix_t *b, int cols);
//int matrix_apply_copy(matrix_t *dst, matrix_t *src);
int matrix_apply_copy(matrix_t *dst, matrix_t *src, int pos);
void matrix_print(matrix_t* mat);
//float matrix_get(const matrix_t *mat, int row, int col);
//void matrix_set(matrix_t *mat, int row, int col, float value);

int get_shape_size(shape_t *shape);
void shape_print(shape_t shape);


matrix_t *matrix_multiply(const matrix_t *mat1, const matrix_t *mat2);

struct matrix3d_t{
    int rows; 
    int cols;
    int channels;
    float *data;
};

typedef struct matrix3d_t matrix3d_t;

#endif //  __MATRIX_H__

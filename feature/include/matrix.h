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
	shape_t shape;
    int empty;
    float *data;
};

typedef struct matrix_t matrix_t;

int get_shape_size(shape_t *shape);
void shape_print(shape_t shape);
float matrix_get(matrix_t *mat, int num_dims, ...);
int matrix_set(matrix_t *mat, float value, int num_dims, ...);


matrix_t *matrix_alloc(int num_dims, ...);
matrix_t *matrix_alloc_shape(shape_t shape);
matrix_t *matrix_empty(int num_dims, ...);
matrix_t *matrix_empty_shape(shape_t shape);

void matrix_free(matrix_t *mat);

matrix_t *matrix_copy(matrix_t *src);
int matrix_apply_copy(matrix_t *dst, int dst_pos, matrix_t *src, int src_pos, int len);

void matrix_print(matrix_t* mat, const char *name);
float matrix_log_energy(matrix_t *mat);
int matrix_apply_pow(matrix_t *mat, float value);
int matrix_apply_floor(matrix_t *mat, float value);
int matrix_apply_log(matrix_t *mat);

int matrix_zero(matrix_t *mat, int pos, int len);
int matrix_resize(matrix_t *mat, shape_t shape);

int matrix_apply_mul_element(matrix_t *mat1, matrix_t *mat2);

float matrix_energy(matrix_t *mat1, matrix_t *mat2);

int matrix_apply_add(matrix_t *dst, matrix_t *src,  float scalar);
matrix_t* matrix_add(matrix_t *dst, matrix_t *src,  float scalar);

matrix_t *matrix_mul(const matrix_t *mat1, const matrix_t *mat2);
int matrix_apply_mul(const matrix_t *dst, const matrix_t *src);

int matrix_power_spectrum(matrix_t *mat);
int matrix_apply_add_value(matrix_t *mat, float value);
float matrix_sum(matrix_t *mat);

#endif //  __MATRIX_H__

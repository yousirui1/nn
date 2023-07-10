#include "base.h"
#include "matrix.h"

int get_shape_size(shape_t *shape)
{
    int i;
    shape->size = 1;
    for(i = 0; i < shape->num_dims; i++)
    {
        shape->size *= shape->dims[i];
    }
    return shape->size;
}

void shape_print(shape_t shape)
{
    int i;
    printf("shape size: %d [", shape.size);
    for(i = 0; i < shape.num_dims; i++)
    {
        printf("%d ", shape.dims[i]);
    }
    printf("]\n");
}

float matrix_get(matrix_t *mat, int num_dims, ...)
{
    int i;
    va_list args;
    float value = 0.0f;
    shape_t shape;

    shape.num_dims = num_dims;
    va_start(args, num_dims);
    for(i = 0; i < num_dims; i++)
    {
        shape.dims[i] = va_arg(args, int);
    }
    va_end(args);

    switch(num_dims)
    {
        case 1:
            value =  mat->data[shape.dims[0]];
            break;

        case 2:
        {
            float (*data)[mat->shape.dims[1]] = mat->data;
            value = data[shape.dims[0]][shape.dims[1]];
            break;
        }
        case 3:
        {

            float (*data)[mat->shape.dims[1]][mat->shape.dims[2]] = mat->data;
            value = data[shape.dims[0]][shape.dims[1]][shape.dims[2]];
            break;
        }

        case 4:
        {
            float (*data)[mat->shape.dims[1]][mat->shape.dims[2]][mat->shape.dims[3]] = mat->data;
            value = data[shape.dims[0]][shape.dims[1]][shape.dims[2]][shape.dims[3]];
            break;
        }

        default:
            LOG_DEBUG("matrix not support num_dims %d", num_dims);
            return ERROR;
    }

    return value;
}


int matrix_set(matrix_t *mat, float value, int num_dims, ...)
{
    int i;
    va_list args;
    shape_t shape;

    shape.num_dims = num_dims;
    va_start(args, num_dims);
    for(i = 0; i < num_dims; i++)
    {
        shape.dims[i] = va_arg(args, int);
    }
    va_end(args);

    switch(num_dims)
    {
        case 1:
            mat->data[shape.dims[0]] = value;
            break;

        case 2:
        {
            float (*data)[mat->shape.dims[1]] = mat->data;
            data[shape.dims[0]][shape.dims[1]] = value;
            break;
        }
        case 3:
        {

            float (*data)[mat->shape.dims[1]][mat->shape.dims[2]] = mat->data;
            data[shape.dims[0]][shape.dims[1]][shape.dims[2]] = value;
            break;
        }

        case 4:
        {
            float (*data)[mat->shape.dims[1]][mat->shape.dims[2]][mat->shape.dims[3]] = mat->data;
            data[shape.dims[0]][shape.dims[1]][shape.dims[2]][shape.dims[3]] = value;
            break;
        }

        default:
            return ERROR;
    }
    return SUCCESS;
}



matrix_t *matrix_alloc(int num_dims, ...)
{
    matrix_t *out = (struct matrix_t *)malloc(sizeof(matrix_t));
    if(out)
    {
        int i;
        va_list args;
        out->shape.num_dims = num_dims;
        va_start(args, num_dims);
        for(i = 0; i < num_dims; i++)
        {
            out->shape.dims[i] = va_arg(args, int);
        }
        va_end(args);
        get_shape_size(&out->shape);
        out->data = (float *)malloc(sizeof(float) * out->shape.size);
    }
    return out;
}


matrix_t *matrix_alloc_shape(shape_t shape)
{
    matrix_t *out = (struct matrix_t *)malloc(sizeof(matrix_t));
    if(out)
    {
        out->shape = shape;
        get_shape_size(&out->shape);
        out->data = (float *)malloc(sizeof(float) * out->shape.size);
    }
    return out;
}


matrix_t *matrix_empty_shape(shape_t shape)
{
    matrix_t *out = (struct matrix_t *)malloc(sizeof(matrix_t));
    if(out)
    {
        out->shape = shape;
        get_shape_size(&out->shape);
    }
    return out;
}


matrix_t *matrix_empty(int num_dims, ...)
{
    matrix_t *out = (struct matrix_t *)malloc(sizeof(matrix_t));
    if(out)
    {
        int i;
        va_list args;
        out->shape.num_dims = num_dims;
        va_start(args, num_dims);
        for(i = 0; i < num_dims; i++)
        {
            out->shape.dims[i] = va_arg(args, int);
        }
        va_end(args);
        get_shape_size(&out->shape);
    }
    return out;
}


void matrix_free(matrix_t *mat)
{
    if(mat)
    {
        if(mat->data)
        {
            free(mat->data);
        }
        free(mat);
    }
}

matrix_t *matrix_copy(matrix_t *src)
{
    matrix_t *out = matrix_alloc_shape(src->shape);
    if(out)
    {
        memcpy(out->data, src->data, sizeof(float) * src->shape.size);
    }
    return out;
}

int matrix_apply_copy(matrix_t *dst, matrix_t *src, int pos)
{
    if(pos + src->shape.size > dst->shape.size)
    {
        LOG_DEBUG("matrix_apply_copy error pos %d src %d dst %d", pos, src->shape.size, dst->shape.size);
        return ERROR;
    }
    memcpy(dst->data + pos, src->data, sizeof(float) * src->shape.size);
    return SUCCESS;
}

void matrix_print(matrix_t* mat) 
{
    int i, count;
    for(i = 0, count = 0; i < mat->shape.size; i++)
    {
        if(count % 10 == 0)
        {
            printf("line %d: ", count / 10);
        }

        printf("%.4f ", mat->data[i]);
        count ++;

        if(count % 10 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

#if 0
float matrix_get(const matrix_t *mat, int row, int col)
{
    return mat->data[row * mat->cols + col];
}

void matrix_set(matrix_t *mat, int row, int col, float value)
{
    mat->data[row * mat->cols + col] = value;
}
#endif




#if 0
int matrix_apply_sum(matrix_t *dst, matrix_t *src,  float scalar)
{
    int i;
    int size = 0;
    if(src->rows != dst->rows || src->cols != dst->cols)
    {
        LOG_DEBUG("matrix shape error");
        return ERROR;
    }

    size = src->rows * src->cols;
    for(i = 0; i < size; i++)
    {
        dst->data[i] += (src->data[i] * scalar);
    }
    return SUCCESS;
}

matrix_t* matrix_sum(matrix_t *dst, matrix_t *src,  float scalar)
{
    int i;
    int size = 0;
    matrix_t *out = NULL;
    if(src->rows != dst->rows || src->cols != dst->cols)
    {
        LOG_DEBUG("matrix shape error");
        return NULL;
    }
    out = matrix_alloc(src->rows, src->cols);
    if(out)
    {
        size = src->rows * src->cols;
        for(i = 0; i < size; i++)
        {
            out->data[i] = dst->data[i] + (src->data[i] * scalar);
        }
    }
    return NULL;
}
#endif



#if 0
matrix_t *matrix_multiply(matrix_t *a, matrix_t *b, int k)
{
    matrix_t *out = matrix_alloc(a->rows, b->cols);
    if(out)
    {
        gemm(false, false, a->rows, b->cols, k, a->data, b->data, 1.0f, out->data);
        return out;
    }
    return NULL;
}

matrix_t *matrix_normalized(matrix_t *src, matrix_t *mean, matrix_t *variance, 
                                int spatial_dim, int num_channels)
{

}

matrix_t *matrix_multiply(const matrix_t *mat1, const matrix_t *mat2)
{
    int m = mat1->rows;
    int n = mat1->cols;
    int p = mat2->cols;

    int i, j, k;
    matrix_t *output = matrix_alloc(m, p);
    for(i = 0; i < m; i ++)
    {
        for(j = 0; j < p; j++)
        {
            float sum = 0.0;
            for(k = 0; k < n; k++)
            {
                matrix_set(output, i, j, sum);
                sum = matrix_get(mat1, i, k) * matrix_get(mat2, k, j);
            }
            matrix_set(output, i, j, sum);
        }
    }

    return output;
}

#endif



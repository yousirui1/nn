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
    int i;
    va_list args;
    matrix_t *out = (struct matrix_t *)malloc(sizeof(matrix_t));
    if(out)
    {
        out->empty = 0;
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
        out->empty = 0;
        out->shape = shape;
        get_shape_size(&out->shape);
        out->data = (float *)malloc(sizeof(float) * out->shape.size);
    }
    return out;
}

matrix_t *matrix_empty(int num_dims, ...)
{
    int i;
    va_list args;
    matrix_t *out = (struct matrix_t *)malloc(sizeof(matrix_t));
    if(out)
    {
        out->shape.num_dims = num_dims;
        va_start(args, num_dims);
        for(i = 0; i < num_dims; i++)
        {
            out->shape.dims[i] = va_arg(args, int);
        }
        va_end(args);
        get_shape_size(&out->shape);
        out->empty = 1;
    }
    return out;
}

matrix_t *matrix_empty_shape(shape_t shape)
{
    matrix_t *out = (struct matrix_t *)malloc(sizeof(matrix_t));
    if(out)
    {
        out->empty = 1;
        out->shape = shape;
        get_shape_size(&out->shape);
    }
    return out;
}


void matrix_free(matrix_t *mat)
{
    if(mat)
    {
        if(mat->data && mat->empty == 0)
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


int matrix_apply_copy(matrix_t *dst, int dst_pos, matrix_t *src, int src_pos, int len)
{
    if(dst_pos + len > dst->shape.size)
    {
        LOG_DEBUG("matrix_apply_copy error dst_pos %d len %d dst size %d", dst_pos, len, dst->shape.size);
        return ERROR;
    }
    memcpy(dst->data + dst_pos, src->data + src_pos, sizeof(float) * len);
    return SUCCESS;
}

void matrix_print(matrix_t* mat, const char *name) 
{
    if(NULL == mat)
        return;

    if(name)
        printf("%s size %d\n", name, mat->shape.size);

    int i, count;
    for(i = 0, count = 0; i < mat->shape.size; i++)
    {
        if(count % 10 == 0)
        {
            printf("line %d: ", count / 10);
        }

        printf("%.8f ", mat->data[i]);
        count ++;

        if(count % 10 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

float matrix_log_energy(matrix_t *mat)
{
    float energy = 0.0f;
    float epsilon = FLT_EPSILON;
    float max_energy = 0.0f;
    int i = 0;

    assert(mat->shape.num_dims == 1);   //signal frame

    for(i = 0; i < mat->shape.size; i++)
    {
        energy += mat->data[i] * mat->data[i];
    }
    max_energy = (energy > epsilon) ? energy : epsilon;
    return logf(max_energy);
}

int matrix_apply_pow(matrix_t *mat, float value)
{
    int i;
    for(i = 0; i < mat->shape.size; i++)
    {
        mat->data[i] = pow(mat->data[i], value);
    }
    return SUCCESS;
}


int matrix_apply_floor(matrix_t *mat, float value)
{
    int i;
    for(i = 0; i < mat->shape.size; i++)
    {
        //mat->data[i] = floor(mat->data[i]);
        mat->data[i] = mat->data[i] < value ? value : mat->data[i];
    }
    return SUCCESS;
}

int matrix_apply_log(matrix_t *mat)
{
    int i;
    for(i = 0; i < mat->shape.size; i++)
    {
        mat->data[i] = logf(mat->data[i]);
    }
    return SUCCESS;
}

int matrix_zero(matrix_t *mat, int pos, int len)
{
    int i;
    if(mat)
    {
        for(i = pos; i < pos + len; i++)
            mat->data[i] = 0.0f;

        return SUCCESS;
    }
    return ERROR;
}

int matrix_resize(matrix_t *mat, shape_t shape)
{
    if(mat)
    {
        mat->shape = shape;
        get_shape_size(&mat->shape);
        mat->data = realloc(mat->data, mat->shape.size * sizeof(float));
        return SUCCESS;
    }
    return ERROR;
}

int matrix_apply_mul_element(matrix_t *mat1, matrix_t *mat2)
{
    int i;
    if(mat1 && mat2)
    {
        if(mat1->shape.size != mat2->shape.size)
        {
            LOG_DEBUG("mat size %d mat size %d no eq", mat1->shape.size, mat2->shape.size);
            return ERROR;
        }
        for(i = 0; i < mat1->shape.size; i++)
        {
            mat1->data[i] = mat1->data[i] * mat2->data[i];;
        }
        return SUCCESS;
    }
    return ERROR;
}

float matrix_energy(matrix_t *mat1, matrix_t *mat2)
{
    int i;
    float value = 0.0f;
    if(mat1->shape.size != mat2->shape.size)
        return 0.0f;

    for(i = 0; i < mat1->shape.size; i++)
    {
        value += mat1->data[i] * mat2->data[i];
    }
    return value;
}

int matrix_apply_add(matrix_t *dst, matrix_t *src,  float scalar)
{
    int i;

    if(dst && src)
    {
        if(src->shape.size != dst->shape.size)
        {
            LOG_DEBUG("matrix shape error");
            return ERROR;
        }

        for(i = 0; i < src->shape.size; i++)
        {
            dst->data[i] += (src->data[i] * scalar);
        }
        return SUCCESS;
    }
    return ERROR;
}

matrix_t* matrix_add(matrix_t *mat1, matrix_t *mat2,  float scalar)
{
    int i;
    if(mat1 && mat2)
    {
        if(mat1->shape.size != mat2->shape.size)
        {
            LOG_DEBUG("matrix shape mat1 %d mat2 %derror", mat1->shape.size, mat2->shape.size);
            return NULL;
        }

        matrix_t *out = matrix_alloc_shape(mat1->shape);
        for(i = 0; i < mat1->shape.size; i++)
        {
            out->data[i] =  mat1->data[i] + mat2->data[i];
        }
        return out;
    }
    return NULL;
}

matrix_t *matrix_mul(const matrix_t *mat1, const matrix_t *mat2)
{
    return NULL;
}

int matrix_apply_mul(const matrix_t *dst, const matrix_t *src)
{
    return SUCCESS;
}

int matrix_power_spectrum(matrix_t *mat)
{
    int32_t i;
    int32_t dim = mat->shape.size;
    int32_t half_dim = dim / 2;

    float first_energy = mat->data[0] * mat->data[0];
    float last_energy = mat->data[1] * mat->data[1];
    float real = 0.0f, im = 0.0f;

    /*  
     * now we have in waveform, first half of complex spectrum
     * it's stored as [real0, realN/2, real1, im1, real2, im2, ...]
     */
    for(i = 1; i < half_dim; i++)
    {   
        real = mat->data[i * 2]; 
        im = mat->data[i * 2 + 1]; 

        mat->data[i] = real * real + im * im; 
    }   
    mat->data[0] = first_energy;
    mat->data[half_dim] = last_energy;
    return SUCCESS;
}

int matrix_apply_add_value(matrix_t *mat, float value)
{
    int i;
    if(mat)
    {
        for(i = 0; i < mat->shape.size; i++)
        {   
            mat->data[i] += value;
        } 
        return SUCCESS;
    }
    return ERROR;
}

float matrix_sum(matrix_t *mat)
{
    int i;
    float value = 0.0f;
    if(mat)
    {
        for(i = 0; i < mat->shape.size; i++)
        {   
            value += mat->data[i];
        } 
    }
    return value;
}

#include "base.h"
#include "matrix.h"

#if 0
static void test_sample()
{
    LOG_DEBUG("==========TestSimple()=============");
    matrix_t *v = matrix_alloc(1, 100000);

    LOG_DEBUG("v->shape.size %d ", v->shape.size);
    for(int i = 0; i < v->shape.size; i++)
    {
        v->data[i] = (abs( i * 433024253 ) % 65535) - (65535 / 2);
    }



}

int main(int argc, char *argv[])
{

    test_sample();
}
#endif

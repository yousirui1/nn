#ifndef __GEMM_H__
#define __GEMM_H__


#include <stdbool.h>

void gemm_bin(int iM, int iN, int iK, float ALPHA, 
        char  *cA, int lda, 
        float *fB, int ldb,
        float *fC, int ldc);
        
void gemm(int TA, int TB, int iM, int iN, int iK, float ALPHA, 
                    float *fA, int lda, 
                    float *fB, int ldb,
                    float BETA,
                    float *fC, int ldc);

/*
**  功能：矩阵计算，完成C = ALPHA * A * B + BETA * C，
**       输出的C也是按行存储（所有行并成一行）
**  输入： A,B,C   输入矩阵（一维数组格式，按行存储，所有行并成一行）
**        ALPHA   系数
**        BETA    系数
**        TA,TB   是否需要对A,B做转置操作，是为1,否为0
**        M       A,C的行数
**        N       B,C的列数
**        K       A的列数，B的行数
**        lda     A的列数（不做转置）或者行数（做转置）
**        ldb     B的列数（不做转置）或者行数（做转置）
**        ldc     C的列数
**  说明：此函数是用C实现矩阵乘法运算，这部分代码应该是模仿的Caffe中的math_functions.cpp的代码
**       参考博客：http://www.voidcn.com/blog/thy_2014/article/p-6149690.html
*/
void gemm_cpu(int TA, int TB, int iM, int iN, int iK, float ALPHA, 
        float *fA, int lda, 
        float *fB, int ldb,
        float BETA,
        float *fC, int ldc);

#endif //  __GEMM_H__

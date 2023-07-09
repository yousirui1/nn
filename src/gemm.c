#include "base.h"
#include "gemm.h"

/*
** 该函数只是调用了gemm_cpu()函数，并且将参数原封不动的传给gemm_cpu()
*/
void gemm(int TA, int TB, int iM, int iN, int iK, float ALPHA, 
        float *fA, int lda, 
        float *fB, int ldb,
        float BETA,
        float *fC, int ldc)
{
    gemm_cpu( TA,  TB,  iM, iN, iK, ALPHA, fA,lda, fB, ldb,BETA, fC,ldc);
}


void gemm_nn(int iM, int iN, int iK, float ALPHA, 
        float *fA, int lda, 
        float *fB, int ldb,
        float *fC, int ldc)
{
    int i,j,k;
    // 大循环：遍历A的每一行，i表示A的第i行，也是C的第i行
    for(i = 0; i < iM; ++i){
        // 中循环：遍历每一行的所有列，k表示A的第k列，同时表示B的第k行
        for(k = 0; k < iK; ++k){
            // 先计算ALPHA * A（A中每个元素乘以ALPHA）
            register float A_PART = ALPHA* fA[i*lda+k];
            // 内循环：遍历B中所有列，每次大循环完毕，将计算得到A*B一行的结果
            // j是B的第j列，也是C的第j列
            for(j = 0; j < iN; ++j){
                // A中的第i行k列与B中的k行i列对应相乘，因为一个大循环要计算A*B整行之结果，
                // 因此，这里用了一个内循环，并没有直接乘以B[k*ldb+i]
                // 每个内循环完毕，将计算A*B整行的部分结果（A中第i行k列与B所有列第k行所有元素相乘的结果）
                fC[i*ldc+j] += A_PART*fB[k*ldb+j];
            }
        }
    }
}

void gemm_nt(int iM, int iN, int iK, float ALPHA, 
        float *fA, int lda, 
        float *fB, int ldb,
        float *fC, int ldc)
{
    int i,j,k;
    // 大循环：遍历fA的每一行，i表示fA的第i行，也是C的第i行
    for(i = 0; i < iM; ++i){
        // 
        for(j = 0; j < iN; ++j){
            register float sum = 0;
            // 内循环：每次内循环结束，将计算fA中第i行与B中第j列相乘的结果，
            // 也就是得到C[i][j]，因为C也一维化了，且按行存储，所以得到C[i*lda+j]
            // k表示fA的第几列，也表示
            for(k = 0; k < iK; ++k){
                sum += ALPHA*fA[i*lda+k]*fB[j*ldb + k];
            }
            fC[i*ldc+j] += sum;
        }
    }
}


void gemm_tn(int iM, int iN, int iK, float ALPHA, 
        float *fA, int lda, 
        float *fB, int ldb,
        float *fC, int ldc)
{
    int i,j,k;
    for(i = 0; i < iM; ++i){
        for(k = 0; k < iK; ++k){
            register float A_PART = ALPHA*fA[k*lda+i];
            for(j = 0; j < iN; ++j){
                fC[i*ldc+j] += A_PART*fB[k*ldb+j];
            }
        }
    }
}


void gemm_tt(int iM, int iN, int iK, float ALPHA, 
        float *fA, int lda, 
        float *fB, int ldb,
        float *fC, int ldc)
{
    int i,j,k;
    for(i = 0; i < iM; ++i){
        for(j = 0; j < iN; ++j){
            register float sum = 0;
            for(k = 0; k < iK; ++k){
                sum += ALPHA*fA[i+k*lda]*fB[k+j*ldb];
            }
            fC[i*ldc+j] += sum;
        }
    }
}


void gemm_cpu(int TA, int TB, int iM, int iN, int iK, float ALPHA, 
        float *fA, int lda, 
        float *fB, int ldb,
        float BETA,
        float *fC, int ldc)
{
    //printf("cpu: %d %d %d %d %d %f %d %d %f %d\n",TfA, TB, iM, iN, iK, ALPHA, lda, ldb, BETfA, ldc);
    int i, j;
    // 先把BETfA * C计算完了，并将结果存在C中，得到的C将为iM行，iN列（按行存储在一维数组C中）
    for(i = 0; i < iM; ++i){
        for(j = 0; j < iN; ++j){
            fC[i*ldc + j] *= BETA;
        }
    }
    // 根据需要，调用下面四种函数之一
    if(!TA && !TB)
        gemm_nn(iM, iN, iK, ALPHA,fA,lda, fB, ldb,fC,ldc);
    else if(TA && !TB)
        gemm_tn(iM, iN, iK, ALPHA,fA,lda, fB, ldb,fC,ldc);
    else if(!TA && TB)
        gemm_nt(iM, iN, iK, ALPHA,fA,lda, fB, ldb,fC,ldc);
    else
        gemm_tt(iM, iN, iK, ALPHA,fA,lda, fB, ldb,fC,ldc);
}


#include "modMat.h"
#include "spmat.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include "help.h"
#include "eigen.h"

/* calculate k vector and M (nnz) */
void countNnz(modMat *B, FILE *fInput){
    int M = 0, i, k_i, check, *kVector = B->k;
    for(i = 0; i < B->n ; ++i){
        /*read k_i from file*/
        check = fread(&k_i, sizeof(int), 1, fInput);
        checkItemsRead(check, 1, __LINE__,__FILE__);
        *kVector = k_i;
        kVector++;
        M += k_i;
        fseek(fInput, k_i*sizeof(int), SEEK_CUR);
    }
    /* return to k_1 in the file  */
    fseek(fInput, sizeof(int) , SEEK_SET);
    B->M = M;
}

/* create sparse matrix of A form input file */
void createSparseA(modMat *B, FILE *fInput){
    int *indices, *p, i, j, check, *k = B->k;
    double *row;

    row = calloc(B->n,sizeof(double));
    checkAllocation(row,__LINE__,__FILE__);
    indices = calloc(B->n,sizeof(int));
    checkAllocation(indices,__LINE__,__FILE__);

    B->A = spmat_allocate_array(B->n, B->M);

    for(i = 0; i < B->n ; ++i){
        fseek(fInput, sizeof(int),SEEK_CUR);
        check = fread(indices, sizeof(int), *k, fInput);
        checkItemsRead(check,*k,__LINE__,__FILE__);
        p = indices;
        for(j = 0; j < *k ; ++j){
            row[*p] = 1.0;
            p++;
        }
        (B->A)->add_row(B->A, row, i);
        /*initialization of row*/
        p = indices;
        for(j = 0; j < *k ; ++j){
            row[*p] = 0.0;
            p++;
        }

        k++;
    }
}

/* getter for B(i,j) when i is the row and j is the column */
double getB(const modMat *B, int i, int j){
    double A_ij, x;
    A_ij = B->A->getA(B->A,i,j);
    /*calculate (k_i*k_j)/M */
    x = (((B->k)[i]) * ((B->k)[j])) /(double) (B->M);
    return (A_ij - x);
}

/* getter for B^(i,j) when i is the row and j is the column */
double getHatB(const modMat *B, int i, int j, int *g, int gLen, double *f){
    double B_ij, x=0;
    int y;
    B_ij = B->getB(B,i,j);
    /*calculate B_ij-delta_ij*f_i */
    if(i==j){
        for (y = 0; y < gLen; ++y) {
            if(*g == i){
                x = f[y];
            }
            g++;
        }
    }
    return (B_ij - x);
}

void printA(modMat *B){
    int i,j;
    printf("A matrix is:\n");
    for(i=0;i<B->n;i++){
        printf("{ ");
        for(j = 0; j < B->n-1; ++j){
            printf("%.3f ,",B->A->getA(B->A,i,j));
        }
        printf("%.3f }\n",B->A->getA(B->A,i,j));
    }
}

void printB(modMat *B){
    int i,j;
    printf("B matrix is:\n");
    for(i=0;i<B->n;i++){
        printf("{ ");
        for(j = 0; j < B->n-1; ++j){
            printf("%.3f ,",B->getB(B,i,j));
        }
        printf("%.3f }\n",B->getB(B,i,j));
    }
}
/*
void printHatB(modMat *HatB){
    int i,j;
    printf("Hat B matrix is:\n");
    for(i=0;i<HatB->n;i++){
        printf("{ ");
        for(j = 0; j < HatB->n-1; ++j){
            printf("%.3f ,",HatB->getHatB(HatB,i,j));
        }
        printf("%.3f }\n",HatB->getHatB(HatB,i,j));
    }
}*/


/* for v = (v_1,...,v_n) and res = (r_1,...,r_n) vectorAddition(res,v) = (v_1+r_1,...,v_n+r_n)*/
/* result of addition will be placed in res vector */
void vectorAddition(double *res, double *v, int vectorLen){
    int i;
    for (i = 0; i < vectorLen; ++i) {
        *res = *res + *v;
        res++;
        v++;
    }
}
/* for v = (v_1,...,v_n) and res = (r_1,...,r_n) vectorSubtraction(res,v) = (r_1-v_1,...,r_n-v_n)*/
/* result of Subtraction will be placed in res vector */
void vectorSubtraction(double *res, double *v, int vectorLen){
    int i;
    for (i = 0; i < vectorLen; ++i) {
        *res = *res - *v;
        res++;
        v++;
    }
}

/* for v0 = (v0_1,...,v0_n) and v1 = (v1_1,...,v1_n) res = (v0_1*v1_1,...,v0_n*v1_n)*/
/* result of addition will be placed in res vector */
void vectorMult(const double *v0, const double *v1, double *res, int vectorLen){
    int i;
    for (i = 0; i < vectorLen; ++i) {
        *res = *v0 * *v1;
        res++;
        v0++;
        v1++;
    }
}

/* for v = (v_1,...,v_n) and scalar res = (v_1*scalar,...,v_n*scalar)*/
/* result will be placed in res vector */
void vectorScalarMult(const double *v, double scalar, double *res, int vectorLen){
    int i;
    for (i = 0; i < vectorLen; ++i) {
        *res += (scalar * (*v));
        res++;
        v++;
    }
}

/* B_hat[g]*v = A[g]*v - (k^T*v*k)/M - f*I*v + ||B_hat[g]||*I*v */
void multB_hat(const struct _modMat *B, const double *v, double *result, int *g, int gLen){
    double *tmp, scalar;
    tmp = calloc(B->n,sizeof(double));
    checkAllocation(tmp, __LINE__,__FILE__);

    /* calculating A*v */
    B->A->mult(B->A,v,tmp,g,gLen);
    /*add tmp to result*/
    vectorAddition(result, tmp, B->n);

    /* calculating ((k^T*v)/M)* k */
    scalar = (dotProduct(B->k, v, B->n)) / B->M;
    vectorScalarMult(B->k, scalar, tmp, B->n);
    /*add tmp to result*/
    vectorSubtraction(result, tmp, B->n);

    /* calculating f*I*v */
    /*vectorMult(B->f, v, tmp, B->n);*/
    /*add tmp to result*/
    vectorSubtraction(result, tmp, B->n);

    /* calculating ||B_hat||*v */
    vectorScalarMult(v, normCalc(B,g,gLen), tmp, B->n);
    /*add tmp to result*/
    vectorAddition(result, tmp, B->n);

    free(tmp);
}

/* create vector (f_1,f_2,...,f_gLen) , result will be placed in f */
void fCalc(const modMat *B, double *f, int *g, int gLen){
    int i,j, *gOut = g, *gIn = g;
    double sum;

    for (i = 0; i < gLen ; ++i) {
        sum = 0;
        for (j = 0; j < gLen ; ++j) {
            sum += B->getB(B,*gOut,*gIn);
            gIn++;
        }
        *f = sum;
        f++;
        gOut++;
        gIn = g;
    }
}

double normCalc(const struct _modMat *B, int *g, int gLen){
    int i, j, *gOut = g, *gIn;
    double *arr, *p, max, *f;

    /* create f according to g */
    f = calloc(gLen,sizeof(double));
    checkAllocation(f, __LINE__, __FILE__);
    fCalc(B, f, g, gLen);
    printVectorDouble(f,gLen);

    arr = calloc(gLen,sizeof(double));
    checkAllocation(arr, __LINE__, __FILE__);
    /*calculating sum of each column - each cell in arr is the sum of a different column */
    for (i = 0; i < gLen ; ++i) {
        p = arr;
        gIn = g;
        for (j = 0; j < gLen ; ++j) {
            *p += B->getHatB(B,*gOut,*gIn,g,gLen,f);
            gIn++;
            p++;
        }
        gOut++;
    }
    /* finding maximal sum of all columns */
    max = *arr;
    for (i = 0; i < gLen ; ++i) {
        if(*arr > max){
            max = *arr;
        }
        arr++;
    }
    free(arr);
    return max;
}

modMat* modMat_allocate(char *location){
    FILE *fInput;
    int check, numOfNodes;
    modMat *mat;

    fInput = fopen(location,"r");
    checkOpenFile(fInput, location, __LINE__, __FILE__);

    /* assign memory */
    mat = calloc(1,sizeof(modMat));
    checkAllocation(mat, __LINE__,__FILE__);

    /*initialize matrix with size from the file */
    check = fread(&numOfNodes, sizeof(int), 1, fInput);
    checkItemsRead(check,1,__LINE__,__FILE__);
    mat->n = numOfNodes;

    /* create vector k and calculate M */
    mat->k = (int*) calloc(numOfNodes,sizeof(int));
    checkAllocation(mat->k, __LINE__, __FILE__);
    countNnz(mat, fInput);

    /* create sparse matrix of A form input file */
    createSparseA(mat, fInput);

    /* define a getter for B*/
    mat->getB = &getB;

    /* define a getter for B^ */
    mat->getHatB= &getHatB;

    /*define mult function for B^*/
    mat->multB_hat=&multB_hat;

    fclose(fInput);
    return mat;
}


#include "modMat.h"
#include "spmat.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
/*#include "help.h"*/
#include "eigen.h"
#include "math.h"











/* calculate k vector and M (nnz) */
void countNnz(modMat *B, FILE *fInput){
    int M = 0, i, k_i, check, *kVector = B->k,j;
    for(i = 0; i < B->n ; ++i){
        /*read k_i from file*/
        check = fread(&k_i, sizeof(int), 1, fInput);
        /* printf("k_%d=%d\n",i,k_i);*/
        checkItemsRead(check, 1, __LINE__,__FILE__);
        *kVector = k_i;
        kVector++;
        M += k_i;
        for(j=0;j<k_i;j++){ /*$$$$$$$delete$$$$$$$$$$$$$$$*/
            fread(&check, sizeof(int), 1, fInput);
        }
        /*&&&&&&&& on nova should be this line!! &&&&&&&&&&&&*/
        /*fseek(fInput,(k_i*sizeof(int)), SEEK_CUR);*/
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

/*calculating dot product of two rows by g on first vector only!*/
double dotProductByG(const int* row1,const double* row2,int *g, int gLen){
    int i;
    double sum=0;
    for(i=0; i<gLen; i++){
        sum += ((*(row1+*g)) * (*(row2)));
        g++;
        row2++;
    }
    return sum;
}

/* for v = (v_1,...,v_n) and scalar res = (v_1*scalar,...,v_n*scalar) according g on first vector only!*/
/* result will be placed in res vector */
void vectorScalarMultByG(const int *v, double scalar, double *res, int *g,int gLen){
    int i;
    for (i = 0; i < gLen; ++i) {
        *res = (scalar * (*(v+*g)));
        res++;
        g++;
    }
}

/* for v = (v_1,...,v_n) and scalar res = (v_1*scalar,...,v_n*scalar)*/
/* result will be placed in res vector */
void vectorScalarMult(const double *v, double scalar, double *res,int gLen){
    int i;
    for (i = 0; i < gLen; ++i) {
        *res = (scalar * (*v));
        res++;
        v++;
    }
}

/* calculate sum(k_1+k_2+...+k_n) = nnz = M according to g
int calcM(const modMat *B, int *g, int gLen){
    int i,sum;
    for(i=0;i<gLen;i++){
        sum+=(B->k)[*g];
        g++;
    }
    return sum;
}*/

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

double normCalc(const struct _modMat *B, int *g, int gLen, double *f){
    int i, j, *gOut = g, *gIn;
    double *arr, *p, max;

    arr = calloc(gLen,sizeof(double));
    checkAllocation(arr, __LINE__, __FILE__);
    /*calculating sum of each column - each cell in arr is the sum of a different column */
    for (i = 0; i < gLen ; ++i) {
        p = arr;
        gIn = g;
        for (j = 0; j < gLen ; ++j) {
            *p += fabs(B->getHatB(B,*gOut,*gIn,g,gLen,f));
            gIn++;
            p++;
        }
        gOut++;
    }
    /* finding maximal sum of all columns */
    p=arr;
    max = *p;
    for (i = 0; i < gLen ; ++i) {
        if(*p > max){
            max = *p;
        }
        p++;
    }
    free(arr);
    return max;
}

/* calculating 1-norm of B_hat using getter of B*/
void calculateNorm(modMat *B){
    int i, j;
    double max = B->getB(B,0,0), sum = 0;

    for (i = 0; i < B->n ; ++i) {
        sum = 0;
        for (j = 0; j < B->n ; ++j) {
            sum += fabs(B->getB(B,i,j));
        }
        if(sum > max){
            max = sum;
        }
    }
    B->norm = max;
}

/*Calc f vector and norm according to g*/
void updateB_Hat(struct _modMat *B, int *g, int gLen){
    /* create f according to g */
    if(B->last_f != NULL){
        free(B->last_f);
    }
    B->last_f = calloc(gLen,sizeof(double));
    checkAllocation(B->last_f, __LINE__, __FILE__);
    fCalc(B, B->last_f, g, gLen);
    /* B->last_norm = normCalc(B,g,gLen,B->last_f);*/
}

/* B_hat[g]*v = A[g]*v - (k^T*v*k)/M - f*I*v + ||B_hat[g]||*I*v */
void multB_hat(const struct _modMat *B, const double *v, double *result, int *g, int gLen){
    double *tmp, scalar, *f;
    tmp = calloc(gLen,sizeof(double));
    checkAllocation(tmp, __LINE__,__FILE__);
    f=B->last_f;

    /* calculating A*v */
    B->A->mult(B->A,v,result,g,gLen);
    /*printf("aaaaaaaa\n");
    printVectorDouble(result,gLen);
    printf("dddddddd\n");*/

    /* calculating ((k^T*v)/M)* k */
    scalar = (dotProductByG(B->k, v, g,gLen)) / B->M;
    vectorScalarMultByG(B->k, scalar, tmp,g, gLen);
    /*add tmp to result*/
    vectorSubtraction(result, tmp, gLen);

    /* calculating f*I*v */
    vectorMult(f, v, tmp, gLen);
    /*add tmp to result*/
    vectorSubtraction(result, tmp, gLen);

    /* calculating ||B_hat||*v */
    vectorScalarMult(v,B->norm, tmp, gLen);
    /*add tmp to result*/
    vectorAddition(result, tmp, gLen);
    free(tmp);
}

/* B_hat[g]*v = A[g]*v - (k^T*v*k)/M - f*I*v + ||B_hat[g]||*I*v */
void multB_hat_noShift(const struct _modMat *B, const double *v, double *result, int *g, int gLen){
    double *tmp, scalar, *f;
    tmp = calloc(gLen,sizeof(double));
    checkAllocation(tmp, __LINE__,__FILE__);
    f=B->last_f;

    /* calculating A*v */
    B->A->mult(B->A,v,result,g,gLen);
    /*printf("aaaaaaaa\n");
    printVectorDouble(result,gLen);
    printf("dddddddd\n");*/

    /* calculating ((k^T*v)/M)* k */
    scalar = (dotProductByG(B->k, v, g,gLen)) / B->M;
    vectorScalarMultByG(B->k, scalar, tmp,g, gLen);
    /*add tmp to result*/
    vectorSubtraction(result, tmp, gLen);

    /* calculating f*I*v */
    vectorMult(f, v, tmp, gLen);
    /*add tmp to result*/
    vectorSubtraction(result, tmp, gLen);

    free(tmp);
}

void freeModMath(modMat *mat){
    mat->A->free(mat->A);
    free(mat->A);
    free(mat->k);
    if(mat->last_f != NULL){
        free(mat->last_f);
    }
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

    /*initialize f vector and norm*/
    mat->last_norm=0;
    mat->last_f=NULL;

    /* create sparse matrix of A form input file */
    createSparseA(mat, fInput);

    /* define a getter for B*/
    mat->getB = &getB;

    /* define a getter for B^ */
    mat->getHatB= &getHatB;

    /*initialize norm of B */
    calculateNorm(mat);

    /*define function for updating f vector and norm according to g*/
    mat->updateB_Hat=&updateB_Hat;

    /*define mult function for B^*/
    mat->multB_hat=&multB_hat;

    /*define free function*/
    mat->freeModMath=&freeModMath;

    /*????????deleteeee ??????*/
    mat->multB_hat_noShift=&multB_hat_noShift;

    fclose(fInput);
    return mat;
}
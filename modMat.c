#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include <sys/stat.h>
#include "modMat.h"
#include "spmat.h"
#include "errors.h"


/*function declaration*/
double getter_B(const modMat *B, int i, int j);
double getter_HatB(const modMat *B, int i, int j, int *g, int gLen);
void mult_HatB(const modMat *B, const double *v, double *result, int *g, int gLen);
/*void multB_hat_noShift(const struct _modMat *B, const double *v, double *result, int *g, int gLen)*/ /*deleteeeeeeeeeeee*/
void update_HatB_f_vector(modMat *B, int *g, int gLen);
void free_modMat(modMat *mat);
modMat* modMat_allocate(char *location);
/*void updated_vector_by_indices_with_value(double *vector,int *indices, int indicesLen, double value);*/
void load_data_from_input_file(modMat *B, char *location);
void calc_f_vector(const modMat *B, double *f, int *g, int gLen);
/*double calc_norm(const modMat *B,  double *f, int *g, int gLen);*/ /*deleteeeeeee*/
double calc_norm(modMat *B);
void vector_addition(double *res, double *v, int vectorLen);
void vector_subtraction(double *res, double *v, int vectorLen);
void vector_mult(const double *v0, const double *v1, double *res, int vectorLen);
double dot_product_by_g(const int* row1,const double* row2,int *g, int gLen);
void vector_scalar_mult_by_g(const int *v, double scalar, double *res, int *g,int gLen);
void vector_scalar_mult(const double *v, double scalar, double *res,int gLen);
void printA(modMat *B); /*deleteeeeee*/
void printB(modMat *B);/*deleteeeeeeee*/
/*void printHatB(modMat *HatB);*/ /*delteeeeeeeeeee*/
/*end of functions declaration*/

/* getter for B(i,j) when i is the row and j is the column */
double getter_B(const modMat *B, int i, int j){
    double A_ij, x;
    A_ij = getter_sparse(B->A,i,j);
    /*calculate (k_i*k_j)/M */
    x = (((B->k)[i]) * ((B->k)[j])) /(double) (B->M);
    return (A_ij - x);
}

/*deleteeeee ??????????*/
/* getter for B^(i,j) according to last_f */
double getter_HatB(const modMat *B, int i, int j, int *g, int gLen){
    double B_ij, x=0;
    int y;
    B_ij = getter_B(B,i,j);
    /*calculate B_ij-delta_ij*f_i */
    if(i==j){
        for (y = 0; y < gLen; ++y) {
            if(*g == i){
                x = B->last_f[y];
            }
            g++;
        }
    }
    return (B_ij - x);
}

/* B_hat[g]*v = A[g]*v - (k^T*v*k)/M - f*I*v + ||B_hat[g]||*I*v */
void mult_HatB(const modMat *B, const double *v, double *result, int *g, int gLen){
    double *tmp, scalar;
    tmp = calloc(gLen,sizeof(double));
    checkAllocation(tmp, __LINE__,__FILE__);

    /* calculating A*v */
    mult_sparse(B->A,v,result,g,gLen);

    /* calculating ((k^T*v)/M)* k */
    scalar = (dot_product_by_g(B->k, v, g,gLen)) / B->M;
    vector_scalar_mult_by_g(B->k, scalar, tmp,g, gLen);
    /*add tmp to result*/
    vector_subtraction(result, tmp, gLen);

    /* calculating f*I*v */
    vector_mult(B->last_f, v, tmp, gLen);
    /*add tmp to result*/
    vector_subtraction(result, tmp, gLen);

    /* calculating ||B_hat||*v */
    vector_scalar_mult(v,B->norm, tmp, gLen);
    /*add tmp to result*/
    vector_addition(result, tmp, gLen);
    free(tmp);
}

/* B_hat[g]*v = A[g]*v - (k^T*v*k)/M - f*I*v + ||B_hat[g]||*I*v */
/*deleteeeeeeeeeeee ?????????*/
/*void multB_hat_noShift(const struct _modMat *B, const double *v, double *result, int *g, int gLen){
    double *tmp, scalar, *f;
    tmp = calloc(gLen,sizeof(double));
    checkAllocation(tmp, __LINE__,__FILE__);
    f=B->last_f;

    calculating A*v
    mult_sparse(B->A,v,result,g,gLen);

     calculating ((k^T*v)/M)* k
    scalar = (dotProductByG(B->k, v, g,gLen)) / B->M;
    vectorScalarMultByG(B->k, scalar, tmp,g, gLen);
    add tmp to result
    vectorSubtraction(result, tmp, gLen);

     calculating f*I*v
    vectorMult(f, v, tmp, gLen);
    add tmp to result
    vectorSubtraction(result, tmp, gLen);

    free(tmp);
}*/

/*Calc f vector according to g*/
void update_HatB_f_vector(modMat *B, int *g, int gLen){
    /* create f according to g */
    if(B->last_f != NULL){
        free(B->last_f);
    }
    B->last_f = calloc(gLen,sizeof(double));
    checkAllocation(B->last_f, __LINE__, __FILE__);
    calc_f_vector(B, B->last_f, g, gLen);
}

/*Free all allocations*/
void free_modMat(modMat *mat){
    free_sparse(mat->A);
    free(mat->A);
    free(mat->k);
    if(mat->last_f != NULL){
        free(mat->last_f);
    }
}

/* Allocates new modularity matrix from input file */
modMat* modMat_allocate(char *location){
    /* assign memory */
    modMat *mat;
    mat = calloc(1,sizeof(modMat));
    checkAllocation(mat, __LINE__,__FILE__);

    /* load all data from input file: n(number of nodes), k vector, M(nnz) and create sparse matrix of A*/
    load_data_from_input_file(mat,location);

    /*initialize norm of B */
    mat->norm=calc_norm(mat);

    /*initialize f vector and norm*/
    mat->last_f=NULL;

    return mat;
}

/*update the vector's values in specific indexes to the same value*/
/*
void updated_vector_by_indices_with_value(double *vector,int *indices, int indicesLen, double value){
    int i=0;
    for(i = 0; i < indicesLen ; ++i){
        vector[*indices] = value;
        indices++;
    }
}*/

/* load all data from input file: n(number of nodes), k vector, M(nnz) and create sparse matrix of A*/
void load_data_from_input_file(modMat *B, char *location){
    int i, k_i, check, *kVector, *indices;
    double *row;
    struct stat info;
    FILE *fInput;

    fInput = fopen(location,"r");
    checkOpenFile(fInput, location, __LINE__, __FILE__);

    /*initialize the matrix with size (n) from the file */
    check = fread(&(B->n),sizeof(int), 1, fInput);
    checkItemsRead(check,1,__LINE__,__FILE__);

    /* create vector k and calculate M */
    B->k = (int*) calloc(B->n,sizeof(int));
    checkAllocation(B->k, __LINE__, __FILE__);
    kVector = B->k;

    /*calc NNZ*/
    stat(location, &info);
    B->M = (info.st_size/sizeof(int)) - (B->n+1);

    /*Initialization of sparse matrix and data vectors*/
    row = calloc(B->n,sizeof(double));
    checkAllocation(row,__LINE__,__FILE__);
    indices = calloc(B->n,sizeof(int));
    checkAllocation(indices,__LINE__,__FILE__);
    B->A = spmat_allocate(B->n, B->M);

    for(i = 0; i < B->n ; ++i){
        /*read k_i from file*/
        check = fread(&k_i, sizeof(int), 1, fInput);
        checkItemsRead(check, 1, __LINE__,__FILE__);
        *kVector = k_i;
        kVector++;

        /*Read the k_i vertexes from file*/
        check = fread(indices, sizeof(int), k_i, fInput);
        checkItemsRead(check,k_i,__LINE__,__FILE__);

        /*Add indices row to sparse matrix A*/
        add_row_sparse(B->A,indices,k_i,i);
    }
    fclose(fInput);
}

/* create vector (f_1,f_2,...,f_gLen) according to g, result will be placed in f */
void calc_f_vector(const modMat *B, double *f, int *g, int gLen){
    int i,j, *gOut = g, *gIn = g;
    double sum;

    for (i = 0; i < gLen ; ++i) {
        sum = 0;
        for (j = 0; j < gLen ; ++j) {
            sum += getter_B(B,*gOut,*gIn); /*@@@@change@@@@*/
            gIn++;
        }
        *f = sum;
        f++;
        gOut++;
        gIn = g;
    }
}

/*delteeeeeeeee?*/
/* calculate 1-norm of B^[g]*/
/*double calc_norm(const modMat *B,  double *f, int *g, int gLen){
    int i, j, *gOut = g, *gIn;
    double *arr, *p, max;

    arr = calloc(gLen,sizeof(double));
    checkAllocation(arr, __LINE__, __FILE__);
    calculating sum of each column - each cell in arr is the sum of a different column
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
     finding maximal sum of all columns
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
}*/

/* calculating 1-norm of B_hat using getter of B*/
double calc_norm(modMat *B){
    int i, j;
    double max = getter_B(B,0,0), sum = 0;

    for (i = 0; i < B->n ; ++i) {
        sum = 0;
        for (j = 0; j < B->n ; ++j) {
            sum += fabs(getter_B(B,i,j));
        }
        if(sum > max){
            max = sum;
        }
    }
    return max;
}

/* for v = (v_1,...,v_n) and res = (r_1,...,r_n) vectorAddition(res,v) = (v_1+r_1,...,v_n+r_n)*/
/* result of addition will be placed in res vector */
void vector_addition(double *res, double *v, int vectorLen){
    int i;
    for (i = 0; i < vectorLen; ++i) {
        *res = *res + *v;
        res++;
        v++;
    }
}

/* for v = (v_1,...,v_n) and res = (r_1,...,r_n) vectorSubtraction(res,v) = (r_1-v_1,...,r_n-v_n)*/
/* result of Subtraction will be placed in res vector */
void vector_subtraction(double *res, double *v, int vectorLen){
    int i;
    for (i = 0; i < vectorLen; ++i) {
        *res = *res - *v;
        res++;
        v++;
    }
}

/* for v0 = (v0_1,...,v0_n) and v1 = (v1_1,...,v1_n) res = (v0_1*v1_1,...,v0_n*v1_n)*/
/* result of addition will be placed in res vector */
void vector_mult(const double *v0, const double *v1, double *res, int vectorLen){
    int i;
    for (i = 0; i < vectorLen; ++i) {
        *res = *v0 * *v1;
        res++;
        v0++;
        v1++;
    }
}

/*calculating dot product of two rows by g on first vector only!*/
double dot_product_by_g(const int* row1,const double* row2,int *g, int gLen){
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
void vector_scalar_mult_by_g(const int *v, double scalar, double *res, int *g,int gLen){
    int i;
    for (i = 0; i < gLen; ++i) {
        *res = (scalar * (*(v+*g)));
        res++;
        g++;
    }
}

/* for v = (v_1,...,v_n) and scalar res = (v_1*scalar,...,v_n*scalar)*/
/* result will be placed in res vector */
void vector_scalar_mult(const double *v, double scalar, double *res,int gLen){
    int i;
    for (i = 0; i < gLen; ++i) {
        *res = (scalar * (*v));
        res++;
        v++;
    }
}

/*delteeeeeee*/
void printA(modMat *B){
    int i,j;
    printf("A matrix is:\n");
    for(i=0;i<B->n;i++){
        printf("{ ");
        for(j = 0; j < B->n-1; ++j){
            printf("%.3f ,",getter_sparse(B->A,i,j));
        }
        printf("%.3f }\n",getter_sparse(B->A,i,j));
    }
}

/*delteeeeeee*/
void printB(modMat *B){
    int i,j;
    printf("B matrix is:\n");
    for(i=0;i<B->n;i++){
        printf("{ ");
        for(j = 0; j < B->n-1; ++j){
            printf("%.3f ,",getter_B(B,i,j));
        }
        printf("%.3f }\n",getter_B(B,i,j));
    }
}

/*delteeeeeee*/
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



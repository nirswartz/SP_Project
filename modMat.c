#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include <sys/stat.h>
#include "modMat.h"
#include "spmat.h"
#include "errors.h"

#define UPPER_BOUND(x) (20 * (x) + 50000)

/*function declaration*/
modMat* modMat_allocate(char *location);
double getter_B(const modMat *B, int i, int j);
void mult_HatB(const modMat *B, const double *v, double *result, int *g, int gLen);
void update_HatB_vectors(modMat *B, int *g, int gLen);
void free_modMat(modMat *mat);
void load_data_from_input_file(modMat *B, char *location);
void calc_f_vector_and_Ag(const modMat *B, int *g, int gLen);
double calc_norm(modMat *B);
double dot_product_by_g(const int* row1, const double* row2,int *g, int gLen);
int count_nnz_by_g(spmat *A, int *g, int gLen);
/*end of functions declaration*/


/* Allocates new modularity matrix from input file */
modMat* modMat_allocate(char *location){
    /* assign memory */
    modMat *mat;
    mat = calloc(1,sizeof(modMat));
    check_allocation(mat, __LINE__,__FILE__);

    /* load all data from input file: n(number of nodes), k vector, M(nnz) and create sparse matrix of A*/
    load_data_from_input_file(mat,location);

    /*initialize norm of B */
    mat->norm = calc_norm(mat);

    /*initialize f vector and A[g] sparse matrix*/
    mat->last_f = NULL;
    mat->last_Ag = NULL;

    /* Defines the upper bound of iterations in order to avoid infinite loops*/
    mat->upper_bound = UPPER_BOUND(mat->n);

    return mat;
}

/* getter for B(i,j) when i is the row and j is the column */
double getter_B(const modMat *B, int i, int j){
    double A_ij, x;
    A_ij = getter_sparse(B->A,i,j);
    /*calculate (k_i*k_j)/M */
    x = (((B->k)[i]) * ((B->k)[j])) /(double) (B->M);
    return (A_ij - x);
}

/* B_hat[g]*v = A[g]*v - (k^T*v*k)/M - f*I*v + ||B_hat[g]||*I*v */
void mult_HatB(const modMat *B, const double *v, double *result, int *g, int gLen){
    double scalar , *f = B->last_f;
    int i, *k = B->k;

    /* calculating A*v */
    mult_sparse(B->last_Ag,v,result);

    /* calculating ((k^T*v)/M)* k */
    scalar = (dot_product_by_g(B->k, v, g,gLen)) / B->M;

    /* calculating simultaneity each coordination in the result vector*/
    for (i = 0; i < gLen; ++i) {
        *result = *result - (scalar * (double)(*(k+*g))) - (*f * *v) + (B->norm * *v);
        result++;
        f++;
        v++;
        g++;
    }

}

/*Calc f vector and A[g] according to g*/
void update_HatB_vectors(modMat *B, int *g, int gLen){
    /* create f according to g */
    if(B->last_f != NULL){
        free(B->last_f);
    }
    if(B->last_Ag != NULL && B->last_Ag->n != B->A->n){
        free_sparse(B->last_Ag);
        free(B->last_Ag);
    }
    B->last_f = calloc(gLen,sizeof(double));
    check_allocation(B->last_f, __LINE__, __FILE__);
    if(gLen == B->n){
        B->last_Ag = B->A;
    }
    else{
        B->last_Ag = spmat_allocate(gLen, count_nnz_by_g(B->A, g, gLen));
    }
    calc_f_vector_and_Ag(B, g, gLen);
}

/*Free all allocations*/
void free_modMat(modMat *mat){
    free_sparse(mat->A);
    free(mat->A);
    free(mat->k);
    if(mat->last_f != NULL){
        free(mat->last_f);
    }
    if(mat->last_Ag !=NULL){
        free_sparse(mat->last_Ag);
        free(mat->last_Ag);
    }

}

/* load all data from input file: n(number of nodes), k vector, M(nnz) and create sparse matrix of A*/
void load_data_from_input_file(modMat *B, char *location){
    int i, k_i, check, *kVector, *indices;
    struct stat info;
    FILE *fInput;

    fInput = fopen(location,"r");
    check_open_file(fInput, location, __LINE__, __FILE__);

    /*initialize the matrix with size (n) from the file */
    check = fread(&(B->n),sizeof(int), 1, fInput);
    check_items_read(check,1,__LINE__,__FILE__);
    check_empty_graph(B->n, __LINE__, __FILE__);

    /* create vector k and calculate M */
    B->k = (int*) calloc(B->n,sizeof(int));
    check_allocation(B->k, __LINE__, __FILE__);
    kVector = B->k;

    /*calc NNZ*/
    stat(location, &info);
    B->M = (info.st_size/sizeof(int)) - (B->n+1);
    /*If There are no edges, all divisions are legit */
    check_no_edges(B->M,__LINE__,__FILE__);

    /*Initialization of sparse matrix and data vectors*/
    indices = calloc(B->n,sizeof(int));
    check_allocation(indices,__LINE__,__FILE__);
    B->A = spmat_allocate(B->n, B->M);

    for(i = 0; i < B->n ; ++i){
        /*read k_i from file*/
        check = fread(&k_i, sizeof(int), 1, fInput);
        check_items_read(check, 1, __LINE__,__FILE__);
        *kVector = k_i;
        kVector++;

        /*Read the k_i vertexes from file*/
        check = fread(indices, sizeof(int), k_i, fInput);
        check_items_read(check,k_i,__LINE__,__FILE__);

        /*Add indices row to sparse matrix A*/
        add_row_sparse(B->A,indices,k_i,i);
    }
    fclose(fInput);
    free(indices);
}

/* create vector (f_1,f_2,...,f_gLen) and A[g] according to g, result will be placed in f */
void calc_f_vector_and_Ag(const modMat *B, int *g, int gLen){
    int i,j, *gOut = g, *gIn = g , *indices, *startIndices, countIndices;
    double A_ij, sum_k_j , *f = B->last_f , sum_row_A_g_ij;

    indices = calloc(gLen,sizeof(int));
    check_allocation(indices,__LINE__,__FILE__);
    startIndices = indices;

    for (i = 0; i < gLen ; ++i) {
        sum_row_A_g_ij = 0.0;
        sum_k_j = 0.0;
        countIndices = 0;
        for (j = 0; j < gLen; ++j) {
            A_ij = getter_sparse(B->A, *gOut, *gIn);
            if(A_ij == 1.0){
                *indices = j;
                indices++;
                countIndices++;
            }
            sum_row_A_g_ij += A_ij;
            sum_k_j += (double) B->k[*gIn];
            gIn++;
        }
        *f = sum_row_A_g_ij - ((B->k[*gOut] / (double)B->M) * sum_k_j);
        f++;
        gOut++;
        gIn = g;
        indices = startIndices;
        if(gLen != B->n){
            add_row_sparse(B->last_Ag,indices,countIndices,i);
        }
    }
    free(indices);
}

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

int count_nnz_by_g(spmat *A, int *g, int gLen){
    int i,j, *gOut = g, *gIn = g , nnz = 0;
    for (i = 0; i < gLen ; ++i) {
        for (j = 0; j < gLen; ++j) {
            if(getter_sparse(A, *gOut, *gIn) == 1.0){
                nnz++;
            }
            gIn++;
        }
        gOut++;
        gIn = g;
    }
    return nnz;
}


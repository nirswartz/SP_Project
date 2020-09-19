#include "spmat.h"
#include <stdio.h>
#ifndef MODMAT_H_
#define MODMAT_H_

typedef struct _modMat {
    /* Matrix size (n*n) */
    int	n;

    /* sparse matrix of A(i,j) */
    spmat *A;

    /* vector of (k_1,k_2,...,k_n) when k_i is the degree of vertex i in G */
    int	*k;

    /* sum(k_1+k_2+...+k_n) also equals to non zero elements in A (nnz) */
    int M;

    /* 1-norm of original B*/
    double norm;

    /*f vector according to the last B^[g]*/
    double *last_f;

    /* A[g] vector according to the last B^[g].
     * This field helps us to accelerate sparse matrix multiplication */
    spmat *last_Ag;

    /* Defines the upper bound of iterations in order to avoid infinite loops*/
    int upper_bound;

} modMat;

/* getter for B(i,j)  */
double getter_B(const modMat *B, int i, int j);

/* Multiplies matrix B_hat by vector v, into result (result is pre-allocated) */
/* B_hat*v = A*v - (k^T*v*k)/M - f*I*v + ||B_hat||*I*v
 * Should call "updateB_Hat" before using this function for new B[g]*/
void mult_HatB(const modMat *B, const double *v, double *result, int *g, int gLen);

/*Calc f vector and A[g] according to g*/
void update_HatB_vectors(modMat *B, int *g, int gLen);

/*Free all allocations*/
void free_modMat(modMat *mat);

/* Allocates new modularity matrix from input file */
modMat* modMat_allocate(char *location);

#endif
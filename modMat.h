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
    /* vector of (f_1,f_2,...,f_n) */
    double *f;
    /* 1-norm of B^[g] */
    double norm;

} modMat;

/* Allocates new modularity matrix from input file */
modMat* modMat_allocate(FILE *fInput);


#endif
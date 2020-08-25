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
    /* getter for B(i,j)  */
    double (*getB)(const struct _modMat *B, int i, int j);
    /* getter for B^(i,j) */
    double (*getHatB)(const struct _modMat *B, int i, int j, int *g, int gLen, double *f);
    /* Multiplies matrix B_hat by vector v, into result (result is pre-allocated) */
    /* B_hat*v = A*v - (k^T*v*k)/M - f*I*v + ||B_hat||*I*v */
    void	(*multB_hat)(const struct _modMat *B, const double *v, double *result, int *g, int gLen);

} modMat;

/* Allocates new modularity matrix from input file */
modMat* modMat_allocate(char *location);

/* calculate 1-norm of B^[g] */
double normCalc(const struct _modMat *B, int *g, int gLen);

/* calculate vector (f_1,f_2,...,f_gLen) according to g */
void fCalc(const modMat *B, double *f, int *g, int gLen);

void printB(modMat *B);

/*void printHatB(modMat *HatB);*/



#endif

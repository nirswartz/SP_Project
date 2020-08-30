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
    /*f vector and norm according to the last B^[g]*/
    double *last_f;
    double last_norm;
    /* getter for B(i,j)  */
    double (*getB)(const struct _modMat *B, int i, int j);
    /* getter for B^(i,j) */
    double (*getHatB)(const struct _modMat *B, int i, int j, int *g, int gLen, double *f);
    /* Multiplies matrix B_hat by vector v, into result (result is pre-allocated) */
    /* B_hat*v = A*v - (k^T*v*k)/M - f*I*v + ||B_hat||*I*v
     * Should call "updateB_Hat" before using this function for new B[g]*/
    void (*multB_hat)(const struct _modMat *B, const double *v, double *result, int *g, int gLen);
    /*Calc f vector and norm according to g*/
    void (*updateB_Hat)(struct _modMat *B, int *g, int gLen);

    /*??????????delete???????*/
    void (*multB_hat_noShift)(const struct _modMat *B, const double *v, double *result, int *g, int gLen);

    /*Free all allocations*/
    void (*freeModMath)(struct _modMat *mat);

} modMat;

/* Allocates new modularity matrix from input file */
modMat* modMat_allocate(char *location);

/* calculate vector (f_1,f_2,...,f_gLen) according to g */
void fCalc(const modMat *B, double *f, int *g, int gLen);

/* calculate sum(k_1+k_2+...+k_n) = nnz = M according to g
int CalcM(const modMat *B, int *g, int gLen);*/

/* calculate 1-norm of B^[g] */
double normCalc(const struct _modMat *B, int *g, int gLen, double *f);

void printB(modMat *B);

/*void printHatB(modMat *HatB);*/



#endif

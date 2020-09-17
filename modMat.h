#ifndef MODMAT_H_
#define MODMAT_H_
#include <stdio.h>
#include "spmat.h"


typedef struct _modMat {
    /* Matrix size (n*n) */
    int	n;

    /* sparse matrix representing the adjacency matrix */
    spmat *A;

    /* vector of (k_1,k_2,...,k_n) when k_i is the degree of vertex i in the graph */
    int	*k;

    /* sum(k_1+k_2+...+k_n) also equals to non zero elements in A (nnz) */
    int M;

    /* 1-norm of original B*/
    double norm;

    /*f vector according to the last B^[g]*/
    double *last_f;

    /* temp calculation vector according to the last B^[g] dimension
     * this vector helps us to decrease allocations in two function:
     * The first is mult_HatB as tmp vector
     * The second is max_division as scores vector*/
    double *calc_double_vector;

} modMat;

/* Allocates new modularity matrix from input file */
modMat* modMat_allocate(char *location);

/* getter for B(i,j)  */
double getter_B(const modMat *B, int i, int j);

/* Multiplies matrix B_hat by vector v, into result (result is pre-allocated) */
/* B_hat*v = A*v - (k^T*v*k)/M - f*I*v + ||B_hat||*I*v
 * Should call "updateB_Hat" before using this function for new B[g]*/
void mult_HatB(const modMat *B, const double *v, double *result, int *g, int gLen);

/* Update calc_double_vector size and calculate f vector according to g*/
void update_HatB_vectors(modMat *B, int *g, int gLen);

/*Free all allocations*/
void free_modMat(modMat *mat);

#endif
#ifndef _SPMAT_H
#define _SPMAT_H

typedef struct _spmat {
    /* Matrix size (n*n) */
    int	n;
    /* Private field for inner implementation.
     * Should not be read or modified externally */
    double	*values;
    int *col, *row;
} spmat;

/* Adds row i the matrix. Called before any other call,
 * exactly n times in order (i = 0 to n-1) */
void add_row_sparse(spmat *A, const double row[], int i);

/* Frees all resources used by A */
void free_sparse(spmat *A);

/* Multiplies matrix A by vector v, into result (result is pre-allocated) */
void mult_sparse(const spmat *A, const double *v, double *result, int *g, int gLen);

/* getter A[i][j]*/
double getter_sparse(const spmat *A, int i, int j);

/* Allocates a new arrays sparse matrix of size n with nnz non-zero elements*/
spmat* spmat_allocate(int n, int nnz);

/* delete*/
void print_sparse(spmat *A);

#endif
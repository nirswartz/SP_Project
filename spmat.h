#ifndef _SPMAT_H
#define _SPMAT_H

/* This class implements a sparse matrix with arrays representation,
 * All values in our project are 0 or 1, so values array is useless.
* for each row, the columns indexes are in increasing order*/

typedef struct _spmat {
    /* Private field for inner implementation.
     * Should not be read or modified externally */

    /* Matrix size (n*n) */
    int	n;

    /* The colind vector - holds the columns indices of nnz values*/
    int *col;

    /* The rowptr vector - holds the number of nnz values from the first row to the current row*/
    int *rowCount;
} spmat;

/* Allocates a new arrays sparse matrix of size n with nnz non-zero elements*/
spmat* spmat_allocate(int n, int nnz);

/* Adds row rowNum to the matrix according to indices.
 * Called before any other call, exactly n times in order (rowNum = 0 to n-1) */
void add_row_sparse(spmat *A, int *indices, int indicesLen, int rowNum);

/* Frees all resources allocated by A */
void free_sparse(spmat *A);

/* Multiplies matrix A[g] by vector v, into result (result is pre-allocated)
 * A[g] is created according to g before calling this function
 * len(v) = len(result) = dim(A[g])*/
void mult_sparse(const spmat *A, const double *v, double *result);

/* getter A[i][j]*/
double getter_sparse(const spmat *A, int i, int j);

#endif
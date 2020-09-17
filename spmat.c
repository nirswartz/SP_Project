#include <stdio.h>
#include <stdlib.h>
#include "spmat.h"
#include "errors.h"

/* This class implements a sparse matrix with arrays representation,
 * All values in our project are 0 or 1, so values array is useless.
* for each row, the columns indexes are in increasing order*/

/* functions deceleration */
spmat* spmat_allocate(int n, int nnz);
void add_row_sparse(spmat *A, int *indices, int indicesLen, int rowNum);
void free_sparse(spmat *A);
void mult_sparse(const spmat *A, const double *v, double *result, int *g, int gLen);
double getter_sparse(const spmat *A, int i, int j);
/*end of functions declaration*/

/* Allocates a new arrays sparse matrix of size n with nnz non-zero elements*/
spmat* spmat_allocate(int n, int nnz){
    spmat *sparse = malloc(sizeof(spmat));
    check_allocation(sparse, __LINE__,__FILE__);
    sparse->n = n;
    sparse->col = calloc(nnz,sizeof(int));
    check_allocation(sparse->col, __LINE__,__FILE__);
    sparse->rowCount = calloc((n+1),sizeof(int));
    check_allocation(sparse->rowCount, __LINE__,__FILE__);
    return sparse;
}

/* Adds row rowNum to the matrix according to indices.
 * Called before any other call, exactly n times in order (rowNum = 0 to n-1) */
void add_row_sparse(spmat *A, int *indices, int indicesLen, int rowNum){
    int *colPtr, i;
    colPtr = (A->col)+(A->rowCount[rowNum]);
    for (i = 0; i < indicesLen ; ++i) {
        *colPtr = *indices;
        colPtr++;
        indices++;
    }
    A->rowCount[rowNum+1] = A->rowCount[rowNum] + indicesLen;
}

/* Frees all resources allocated by A */
void free_sparse(spmat *A){
    free(A->col);
    free(A->rowCount);
}

 /* Multiplies matrix A[g] by vector v, into result (result is pre-allocated)
 * g is a group with glen indices which define A[g]
 * len(v) = len(result) = dim(A[g]) = gLen*/
void mult_sparse(const spmat *A, const double *v, double *result, int *g, int gLen){
    int *colPtr, valuesInRow, i ,j, *skipValues, *gStart;
    double sum;
    const double *vStart;
    gStart = g;
    vStart = v;

    for(i = 0; i < gLen; ++i){
        skipValues = A->rowCount + g[i];
        colPtr = A->col + *skipValues;
        valuesInRow= *(skipValues+1) - *skipValues;
        sum = 0;
        for (j = 0; j < gLen && 0 < valuesInRow ; ++j) {
            while( 1 <= valuesInRow && *colPtr < *g){
                colPtr++;
                valuesInRow--;
            }
            if( 1 <= valuesInRow && *colPtr == *g){
                sum += *v;
                colPtr++;
                valuesInRow--;
            }
            g++;
            v++;
        }
        g = gStart;
        v = vStart;
        *result = sum;
        result++;
    }
}

/* getter for A(i,j) when i is the row and j is the column */
double getter_sparse(const spmat *A, int rowNum, int columnNum){
    int *colPtr, valuesInRow, k, *skipValues;
    skipValues = A->rowCount + rowNum;
    colPtr = (A->col) + *skipValues;
    valuesInRow = *(skipValues + 1) - *skipValues;
    for(k = 0; k < valuesInRow; k++){
        if(*colPtr == columnNum){
            return 1.0;
        }
        if(*colPtr > columnNum){
            return 0.0;
        }
        colPtr++;
    }
    return 0.0;
}
#include <stdio.h>
#include <stdlib.h>
#include "spmat.h"
#include "errors.h"

/* functions deceleration */
void add_row_sparse(spmat *A, int *indices, int indicesLen, int rowNum);
void free_sparse(spmat *A);
void mult_sparse(const spmat *A, const double *v, double *result, int *g, int gLen);
double getter_sparse(const spmat *A, int i, int j);
spmat* spmat_allocate(int n, int nnz);
void print_sparse(spmat *A); /*deleteeeeee*/
/*end of functions declaration*/

/* Adds row i the matrix. Called before any other call,
 * exactly n times in order (i = 0 to n-1) */
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

/* Frees all resources used by A */
void free_sparse(spmat *A){
    free(A->col);
    free(A->rowCount);
}

/* Multiplies matrix A[g] by vector v, into result (result is pre-allocated)
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

/*void mult_sparse(const spmat *A, const double *v, double *result, int *g, int gLen){
    int i,j, *gI = g, *gJ = g;
    const double *p;
    double sum;
    for(i=0;i<gLen;i++) {
        sum=0.0;
        p = v;
        for (j = 0; j < gLen; j++) {
            sum+=((getter_sparse(A,*gI,*gJ))* *p);
            gJ++;
            p++;
        }
        gJ = g;
        gI++;
        result[i]=sum;
    }
}*/

/* getter for A(i,j) when i is the row and j is the column */
double getter_sparse(const spmat *A, int rowNum, int columnNum){
    int *colPtr, ValuesInRow, k, *skipValues;
    skipValues = A->rowCount + rowNum;
    colPtr = (A->col) + *skipValues;
    ValuesInRow = *(skipValues + 1) - *skipValues;
    for(k = 0; k < ValuesInRow; k++){
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


/* getter for A(i,j) when i is the row and j is the column */
/*double getter_sparse(const spmat *A, int i, int j){
    int x, y , *col;

    x = (A->rowCount)[i+1] - (A->rowCount)[i];
    if (x == 0){
        return 0.0;
    }
    move to the relevant position in column array
    col = (A->col) + (A->rowCount)[i];

     look for the relevant value
    for (y = 0; y < x; ++y) {
        if (*col > j){
            break;
        }
        else if (*col == j){
            return 1.0;
        }
        col++;
    }
    return 0.0;
}*/

/* Allocates a new arrays sparse matrix of size n with nnz non-zero elements*/
spmat* spmat_allocate(int n, int nnz){
    spmat *sparse = malloc(sizeof(spmat));
    checkAllocation(sparse, __LINE__,__FILE__);
    sparse->n = n;
    sparse->col = calloc(nnz,sizeof(int));
    checkAllocation(sparse->col, __LINE__,__FILE__);
    sparse->rowCount = calloc((n+1),sizeof(int));
    checkAllocation(sparse->rowCount, __LINE__,__FILE__);
    return sparse;
}

/*to deleteeeeeeeeeeeeeeeeeeee*/
void print_sparse(spmat *A){
    int nnz = *((A->rowCount)+((A->n)));
    int i;
    printf("Col ptr is {");
    for (i = 0; i < nnz-1; ++i) {
        printf("%d,",*((A->col)+i));
    }
    printf("%d}\n",*((A->col)+i));
    printf("Row Count ptr is {");
    for (i = 0; i < A->n; ++i) {
        printf("%d,",*((A->rowCount)+i));
    }
    printf("%d}\n",*((A->rowCount)+i));
}
#include <stdio.h>
#include <stdlib.h>
#include "spmat.h"
#include "errors.h"

void printSparse(struct _spmat *A){
    int nnz = *((A->row)+((A->n)));
    int i;
    printf("{");
    for (i = 0; i < nnz-1; ++i) {
        printf("%f,",*((A->values)+i));
    }
    printf("%f}\n",*((A->values)+i));
    printf("{");
    for (i = 0; i < nnz-1; ++i) {
        printf("%d,",*((A->col)+i));
    }
    printf("%d}\n",*((A->col)+i));
    printf("{");
    for (i = 0; i < A->n; ++i) {
        printf("%d,",*((A->row)+i));
    }
    printf("%d}\n",*((A->row)+i));
}

void add_row(struct _spmat *A, const double *row, int i){
    double *valPtr = (A->values)+(A->row[i]);
    int *colPtr = (A->col)+(A->row[i]);
    int cntNz = 0, col;
    for (col = 0; col < (A->n); ++col) {
        if (*row != 0){
            cntNz++;
            *valPtr = *row;
            *colPtr = col;
            valPtr++;
            colPtr++;
        }
        row++;
    }
    A->row[i+1] = A->row[i] + cntNz;
}

void freeMat(struct _spmat *A){
    free(A->values);
    free(A->col);
    free(A->row);
}


void mult(const struct _spmat *A, const double *v, double *result){
    int *p1 = A->row, *p2 = (A->row)+1,i,j, *colPtr = A->col;
    double *valPtr = A->values, sum;
    for (i = 0; i < (A->n); ++i) {
        sum=0;
        for (j = 0; j < ((*p2) - (*p1)); ++j) {
            sum += (*valPtr * v[*colPtr]);
            valPtr++;
            colPtr++;
        }
        *result = sum;
        result++;
        p1++;
        p2++;
    }
}

double doProductByRow(const struct _spmat *A, int rowNum, const double *v, double *result){
    int numberOfElements = A->row[rowNum+1] - A->row[rowNum], i;
    double *valPtr = (A->values)+(A->row[rowNum]), sum = 0;
    int *colPtr = (A->col)+(A->row[rowNum]);
    for (i = 0; i < numberOfElements; ++i) {
        sum += *valPtr * v[*colPtr];
        valPtr++;
        colPtr++;
    }
    return sum;
}

spmat* spmat_allocate_array(int n, int nnz){
    spmat *sparse = malloc(sizeof(spmat));
    checkAllocation(sparse);
    sparse->n = n;
    sparse->values = (double*) calloc(nnz,sizeof(double));
    checkAllocation(sparse->values);
    sparse->col = (int*) calloc(nnz,sizeof(int));
    checkAllocation(sparse->col);
    sparse->row = (int*) calloc(n+1,sizeof(int));
    checkAllocation(sparse->row);
    sparse->add_row=&add_row;
    sparse->mult=&mult;
    sparse->free=&freeMat;
    sparse->printSparse=&printSparse;
    sparse->doProductByRow=&doProductByRow;
    return sparse;
}
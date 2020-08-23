#include "modMat.h"
#include "spmat.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include "help.h"

/* calculate k vector and M (nnz) */
void countNnz(struct _modMat *B, FILE *fInput){
    int M = 0, i, k_i, check, *kVector = B->k;
    for(i = 0; i < B->n ; ++i){
        /*read k_i from file*/
        check = fread(&k_i, sizeof(int), 1, fInput);
        checkItemsRead(check, 1, __LINE__,__FILE__);
        *kVector = k_i;
        kVector++;
        M += k_i;
        printf("K_%d is %d\n",i,k_i);
        fseek(fInput, (k_i)* sizeof(int)-2, SEEK_CUR); /*ìáãå÷ ùäàéðã÷ñéí ú÷éðéí???*/
    }
    /* return to k_1 in the file  */
    fseek(fInput, sizeof(int) , SEEK_SET);
    B->M = M;
}

/* create sparse matrix of A form input file */
void createSparseA(struct _modMat *B, FILE *fInput){
    int *indices, *p, i, j, check, *k = B->k;
    double *row;
    row = malloc(B->n * sizeof(double));
    indices = malloc(B->n * sizeof(int));

    spmat *A = spmat_allocate_array(B->n, B->M);

    for(i = 0; i < B->n ; ++i){
        fseek(fInput, sizeof(int),SEEK_CUR);
        check = fread(indices, sizeof(int), *k, fInput);
        checkItemsRead(check,*k,__LINE__,__FILE__);
        p = indices;
        for(j = 0; j < *k ; ++j){
            row[*p] = 1.0;
            p++;
        }
        A->add_row(B->A, row, i);
        /*initialization of row*/
        p = indices;
        for(j = 0; j < *k ; ++j){
            row[*p] = 0.0;
            p++;
        }

        k++;
    }
    B->A = A;
}

/* create vector (f_1,f_2,...,f_n) */
void createF(struct _modMat *B){
    int i,j;
    double sum, *f = B->f;

    for (i = 0; i < B->n ; ++i) {
        sum = 0;
        for (j = 0; j < B->n ; ++j) { /*  g - àí îçùáéí òáåø àéðã÷ñéí ñôöéôééí ìùðåú ëàï!!! */
            sum += B->getB(B,i,j);
        }
        *f = sum;
        f++;
    }
}

/* calculating 1-norm of B_hat using getter of B */
void calculateNorm(struct _modMat *B){
    int i, j;
    double *arr, *p, max = B->getB(B,0,0);
    arr = malloc(B->n * sizeof(double));
    checkAllocation(arr, __LINE__, __FILE__);
    /*calculating sum of each column */
    for (i = 0; i < B->n ; ++i) {
        p = arr;
        for (j = 0; j < B->n ; ++j) {
            *p += B->getB(B,i,j);
            p++;
        }
    }
    /* finding maximal sum of all columns */
    for (i = 0; i < B->n ; ++i) {
        if(*arr > max){
            max = *arr;
        }
        arr++;
    }
    free(arr);
    B->norm = max;
}

/* getter for B(i,j) when i is the row and j is the column */
double getB(const struct _modMat *B, int i, int j){
    double A_ij, x;
    A_ij = getA(B->A,i,j);
    /*calculate (k_i*k_j)/M */
    x = ((B->k)[i] * (B->k)[j]) / B->M;
    return (A_ij - x);
}

modMat* modMat_allocate(char *location){
    FILE *fInput;
    int n, numOfNodes;
    modMat *mat;

    fInput = fopen(location,"r");
    checkOpenFile(fInput, location, __LINE__, __FILE__);

    /* assign memory */
    mat = malloc(sizeof(modMat));
    checkAllocation(mat, __LINE__,__FILE__);

    /*initialize matrix with size from the file */
    n = fread(&numOfNodes, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__,__FILE__);
    mat->n = numOfNodes;
    printf("N is %d\n",mat->n);

    /* create vector k and calculate M */
    mat->k = (int*) calloc(n,sizeof(int));
    checkAllocation(mat->k, __LINE__, __FILE__);
    countNnz(mat, fInput);

    /* create sparse matrix of A form input file */
    createSparseA(mat, fInput);

    /* define a getter for B_hat */
    mat->getB = &getB;

    /* create vector (f_1,f_2,...,f_n) */
    mat->f = (double*) calloc(n,sizeof(double));
    createF(mat);

    /* calculate ||B_hat|| (1-norm of B) */
    calculateNorm(mat);

    fclose(fInput);
    return mat;
}
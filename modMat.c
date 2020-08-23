#include "modMat.h"
#include "spmat.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>

/* calculate k vector and M (nnz) */
void countNnz(struct _modMat *B, FILE *fInput){
    int M = 0, i, k_i, check, *kVector = B->k;

    for(i = 0; i < B->n ; ++i){
        /*read k_i from file*/
        check = fread(&k_i, sizeof(int), 1, fInput);
        checkItemsRead(check, 1, __LINE__,__FILE__);
        *kVector = k_i;
        M += k_i;
        fseek(fInput, k_i , SEEK_CUR); /*לבדוק שהאינדקסים תקינים???*/
    }
    /* return to the beginning of the file */
    fseek(fInput, 1 , SEEK_SET);
    B->M = M;
}

/* create sparse matrix of A form input file */
void createSparseA(struct _modMat *B, FILE *fInput){
	int *row, *indices, *p, i, j, check, *k = B->k;
	row = malloc(B->n * sizeof(int));
	indices = malloc(B->n * sizeof(int));

    spmat *A = spmat_allocate_array(B->n, B->M);

    for(i = 0; i < B->n ; ++i){
        fseek(fInput,1,SEEK_CUR);
        check = fread(indices, sizeof(int), *k, fInput);
        checkItemsRead(check,*k,__LINE__,__FILE__);
        p = indices;
        for(j = 0; j < *k ; ++j){
        	row[*p] = 1;
        	p++;
        }
        p = indices;
        for(j = 0; j < *k ; ++j){
        	row[*p] = 0;
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
		for (j = 0; j < B->n ; ++j) { /*  g - אם מחשבים עבור אינדקסים ספציפיים לשנות כאן!!! */
			sum += B->getB(B,i,j);
		}
		*f = sum;
		f++;
	}
}

/* calculating 1-norm of B_hat using getter of B */
double calculateNorm(const struct _modMat *B){
	int i, j;
	double *arr, *p, max = B->getB(B,0,0);
	arr = malloc(B->n * sizeof(int));
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
	return max;
}

/* getter for B(i,j) when i is the row and j is the column */
double getB(const struct _modMat *B, int i, int j){
	double A_ij, x;
	A_ij = getA(B->A,i,j);
	/*calculate (k_i*k_j)/M */
	x = ((B->k)[i] * (B->k)[j]) / B->M;
	return (A_ij - x);
}

modMat* modMat_allocate(FILE *fInput){
	int n, *numOfNodes;
	modMat *mat;

	/* assign memory */
	mat = malloc(sizeof(modMat));
	checkAllocation(mat, __LINE__,__FILE__);

    /*initialize matrix with size from the file */
    n = fread(&numOfNodes, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__,__FILE__);
    mat->n = n;
    /* create vector k and calculate M */
    mat->k = (int*) calloc(n,sizeof(int));
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

	return mat;
}


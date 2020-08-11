#include "modMat.h"
#include "spmat.h"
#include "errors.h"

#include <stdio.h>
#include <stdlib.h>

/* calculate k vector and M (nnz) */
void countNnz(modMat *B, FILE *fInput){
    int M = 0, i, k_i, check, *kVector = B->k;

    for(i = 0; i < B->n ; ++i){
        /*read k_i from file*/
        check = fread(&k_i, sizeof(int), 1, fInput);
        checkItemsRead(check, 1, __LINE__);
        *kVector = k_i;
        M += k_i;
        fseek(fInput, k_i , SEEK_CUR); /*לבדוק שהאינדקסים תקינים???*/
    }
    /* return to the beginning of the file */
    fseek(fInput, 1 , SEEK_SET);
    B->M = M;
}

/* create sparse matrix of A form input file */
void createSparseA(modMat *B, FILE *fInput){
	int *row, *indices, *p, i, j, check, *k = B->k;
	row = malloc(B->n * sizeof(int));
	indices = malloc(B->n * sizeof(int));

    spmat *A = spmat_allocate_array(B->n, B->M);

    for(i = 0; i < B->n ; ++i){
        fseek(fInput,1,SEEK_CUR);
        check = fread(indices, sizeof(int), k, fInput);
        checkItemsRead(check,k,__LINE__);
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

modMat* modMat_allocate(FILE *fInput){
	int n, *numOfNodes;

	/* assign memory */
	modMat *mat = malloc(sizeof(modMat));
	checkAllocation(mat, __LINE__);

    /*initialize matrix with size from the file*/
    n = fread(&numOfNodes, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__);
    mat->n = n;
    /* create vector k and calculate M*/
    mat->k = (int*) calloc(n,sizeof(int));
    countNnz(mat, fInput);
    /* create sparse matrix of A form input file */
    createSparseA(mat, fInput);

	return mat;
}


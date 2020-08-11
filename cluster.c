#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "spmat.h"
#include "errors.h"
#include "eigen.h"
#include "assert.h" /*delete in the end*/

/*delete in the end*/
int checkNNZ(FILE *fInput, int vectorSize){
    int count=0,i,j,n;
    double *matrixRow, *tmp;

    matrixRow = calloc(vectorSize,sizeof(double));
    assert(matrixRow!=NULL);
    for(i = 0; i < vectorSize; ++i){
        /*read line in cov matrix*/
        assert((int)(fread(matrixRow, sizeof(double), vectorSize, fInput))==vectorSize);
        tmp=matrixRow;
        for (j = 0; j < vectorSize; ++j) {
            if (tmp[j]!=0){
                count++;
            }
        }
    }
    rewind(fInput);
    assert(fread(&vectorSize, sizeof(int), 1, fInput)==1); /*just to skip matrix size*/
    assert(fread(&vectorSize, sizeof(int), 1, fInput)==1); /*just to skip matrix size*/
    printf("NNZ=%d\n",count);
    return count;
}

spmat* fileToSparseMatrix(char *location) {
    FILE *fInput;
    int vectorSize, n, i;
    double *matrixRow;
    spmat *matrix;

    /* open files of matrix*/
    fInput = fopen(location, "r");
    checkOpenFile(fInput, location, __LINE__);

    /*initialize matrix with size from the file*/
    n = fread(&vectorSize, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__);
    n = fread(&vectorSize, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__);
    n=checkNNZ(fInput, vectorSize);
    matrix=spmat_allocate_array(vectorSize, n);

    /*initialize row and*/
    matrixRow = calloc(vectorSize,sizeof(double));
    checkAllocation(matrixRow,__LINE__);

    /*Read matrix row from file and add to sprase matrix*/
    for(i = 0; i < vectorSize; ++i){
        /*read line in cov matrix*/
        n=fread(matrixRow, sizeof(double), vectorSize, fInput);
        checkItemsRead(n,vectorSize,__LINE__);
        matrix->add_row(matrix,matrixRow,i);
    }
    fclose(fInput);
    free(matrixRow);
    return matrix;
}

void calcEigenVectorAndWrite(spmat *matrix, char *location){
    FILE *fOutput;
    double *eigenVector;
    int flag=1,n;

    fOutput = fopen(location, "w");
    checkOpenFile(fOutput, location ,__LINE__ );

    /*initialize row and*/
    eigenVector = calloc(matrix->n,sizeof(double));
    checkAllocation(eigenVector,__LINE__);

    /*powerIteration(matrix,eigenVector);*/

    /* write eigenvector with greatest eigenvalue into output file */
    n =fwrite(&flag, sizeof(int), 1, fOutput);
    assert(n==1);
    flag=matrix->n;
    n =fwrite(&flag, sizeof(int), 1, fOutput);
    assert(n==1);
    n = fwrite(eigenVector, sizeof(double), flag, fOutput);
    assert(n==flag);

    fclose(fOutput);
    free(eigenVector);
}

void printFileMatrix(char *location){
    FILE *fInput;
    int vectorLen,VectorWid, n, i,j;
    double *matrixRow;

    /* open files of matrix*/
    fInput = fopen(location, "r");
    checkOpenFile(fInput, location, __LINE__);

    /*initialize row and*/
    matrixRow = calloc(VectorWid,sizeof(double));
    checkAllocation(matrixRow,__LINE__);

    /*initialize matrix with size from the file*/
    n = fread(&vectorLen, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__);
    n = fread(&VectorWid, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__);
    printf("The matrix is %d x %d\n",vectorLen,VectorWid);

    /*Read matrix row from file and add to sprase matrix*/
    for(i = 0; i < vectorLen; ++i){
        /*read line in cov matrix*/
        n=fread(matrixRow, sizeof(double), VectorWid, fInput);
        checkItemsRead(n,VectorWid,__LINE__);
        printf(" %f ,", matrixRow[0]);
        for(j = 1; j < VectorWid-1; ++j){
            printf(" %f ,", matrixRow[j]);
        }
        printf(" %f \n", matrixRow[VectorWid-1]);
    }
    fclose(fInput);
    free(matrixRow);
}

int main(int argc, char* argv[]) {
    char *inputL="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\input5.arr";
    char *outputL="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\outputs\\output5.arr";
    printFileMatrix(inputL);
    printf("\n");
    spmat *matrix=fileToSparseMatrix(inputL);
    matrix->printSparse(matrix);
    calcEigenVectorAndWrite(matrix,outputL);
    printFileMatrix(outputL);
}

/* check sprase matrix
 * int i;
 double arr[3][4] = {{61,0,0},{0,0,0},{0,84,22}};
 double *ptr=arr[0];
 double *ptr1=arr[1];
 double *ptr2=arr[2];
 double ptr3[3]={1,-1,1};
 double brr[3];
 spmat *mys = spmat_allocate_array(3,3);
 mys->add_row(mys, ptr, 0);
 mys->add_row(mys, ptr1, 1);
 mys->add_row(mys, ptr2, 2);
 mys->printSparse(mys);

/* check mult
 * int i;
 double arr[3][3] = {{61,0,0},{0,50,0},{0,84,22}};
 double brr[3] = {1,-1,1};
 double res[3];
 spmat *mys = NULL;

 mys=spmat_allocate_array(3,3);
 for (i = 0; i < 3; ++i) {
     mys->add_row(mys, arr[i], i);
 }

 mys->mult(mys,brr,res);
 printf("{");
 for (i = 0; i < 3; ++i) {
     printf("%f,",*(res+i));
 }
 printf("}\n");
 /*  mys->printSparse(mys);
 return 0;*/
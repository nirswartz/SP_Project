#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "spmat.h"
#include "errors.h"
#include "eigen.h"
#include "assert.h"
#include "help.h"

/* calc NNZ elements of regular adjacency matrix in file location*/
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

/*transfer regular adjacency matrix in file location to sparse matrix*/
spmat* fileToSparseMatrix(char *location) {
    FILE *fInput;
    int vectorSize, n, i;
    double *matrixRow;
    spmat *matrix;

    /* open files of matrix*/
    fInput = fopen(location, "r");
    checkOpenFile(fInput, location, __LINE__,__FILE__);

    /*initialize matrix with size from the file*/
    n = fread(&vectorSize, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__,__FILE__);
    n = fread(&vectorSize, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__,__FILE__);
    n=checkNNZ(fInput, vectorSize);
    matrix=spmat_allocate_array(vectorSize, n);

    /*initialize row and*/
    matrixRow = calloc(vectorSize,sizeof(double));
    checkAllocation(matrixRow,__LINE__,__FILE__);

    /*Read matrix row from file and add to sprase matrix*/
    for(i = 0; i < vectorSize; ++i){
        /*read line in cov matrix*/
        n=fread(matrixRow, sizeof(double), vectorSize, fInput);
        checkItemsRead(n,vectorSize,__LINE__,__FILE__);
        matrix->add_row(matrix,matrixRow,i);
    }
    fclose(fInput);
    free(matrixRow);
    return matrix;
}

/*calc powerIteration of sprase matrix and write the eigen vector to location
 * vector template: (num of rows - int)(num of columns - int)(values - double) */
void calcEigenVectorAndWrite(spmat *matrix, char *location){
    FILE *fOutput;
    double *eigenVector;
    int flag=1,n;

    fOutput = fopen(location, "w");
    checkOpenFile(fOutput, location ,__LINE__,__FILE__ );

    /*initialize row and*/
    eigenVector = calloc(matrix->n,sizeof(double));
    checkAllocation(eigenVector,__LINE__,__FILE__);

    /*calc eigen with power iteration*/
    powerIteration(matrix,eigenVector);

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

/*print regular adjacency matrix in file location*/
void printFileMatrix(char *location){
    FILE *fInput;
    int vectorLen,VectorWid, n, i,j;
    double *matrixRow;

    /* open files of matrix*/
    fInput = fopen(location, "r");
    checkOpenFile(fInput, location, __LINE__,__FILE__);

    /*initialize row and*/
    matrixRow = calloc(VectorWid,sizeof(double));
    checkAllocation(matrixRow,__LINE__,__FILE__);

    /*initialize matrix with size from the file*/
    n = fread(&vectorLen, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__,__FILE__);
    n = fread(&VectorWid, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__,__FILE__);
    printf("The matrix is %d x %d\n",vectorLen,VectorWid);

    /*Read matrix row from file and add to sprase matrix*/
    for(i = 0; i < vectorLen; ++i){
        /*read line in cov matrix*/
        n=fread(matrixRow, sizeof(double), VectorWid, fInput);
        checkItemsRead(n,VectorWid,__LINE__,__FILE__);
        printf(" %f ,", matrixRow[0]);
        for(j = 1; j < VectorWid-1; ++j){
            printf(" %f ,", matrixRow[j]);
        }
        printf(" %f \n", matrixRow[VectorWid-1]);
    }
    fclose(fInput);
    free(matrixRow);
}

/* create new file of regular matrix in location
 * matrix template: (num of rows - int)(num of columns - int)(values - double)*/
void createMatrixFile(double *values,int n, int m, char *location){
    FILE *fOutput;
    int check,i;

    fOutput = fopen(location, "w");
    checkOpenFile(fOutput, location ,__LINE__,__FILE__ );

    check =fwrite(&m, sizeof(int), 1, fOutput);
    assert(check==1);
    check =fwrite(&n, sizeof(int), 1, fOutput);
    assert(check==1);
    for(i=0;i<m;i++){
        check = fwrite(values, sizeof(double), n, fOutput);
        assert(check==n);
        values+=n;
    }
    fclose(fOutput);
}

/* print received vector*/
void printVector(double *vector, int vectorSize){
    int i;
    for(i=0;i<vectorSize;i++){
        printf("%f, ",vector[i]);
    }
    printf("\n");
}

/*tester of power iteration*/
void test1(){
    char *inputL="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\input5.arr";
    char *outputL="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\outputs\\output5.arr";

    printFileMatrix(inputL);
    printf("\n");

    spmat *matrix=fileToSparseMatrix(inputL);
    matrix->printSparse(matrix);
    printf("\n");

    calcEigenVectorAndWrite(matrix,outputL);
    /*printFileMatrix(outputL);*/
}

/*tester of create matrix*/
void test2(){
    /*double values[]={61.0,0.0,0.0,0.0,0.0,0.0,0.0,84.0,22.0};
    char *location="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\example.arr";
    createMatrixFile(values,3,3,location);
    printFileMatrix(location);*/

    double values[3]={13707.000000, 31369.000000, 10098.000000};
    char *location="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\b0.arr";
    createMatrixFile(values,3,1,location);
    printFileMatrix(location);
}
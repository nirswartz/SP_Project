#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "spmat.h"
#include "errors.h"
#include "eigen.h"
#include "assert.h"
#include "help.h"
#include "modMat.h"
#include "linkedList.h"
#include "divide.h"
#include "cluster.h"

/*
#define NIR_INPUT\ C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\
#define NIR_OUTPUT\ C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\outputs\\
#define SONIA_INPUT\ C://project//inputs//
#define SONIA_OUTPUT\ C://project//outputs//
*/

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
    check_open_file(fInput, location, __LINE__,__FILE__);

    /*initialize matrix with size from the file*/
    n = fread(&vectorSize, sizeof(int), 1, fInput);
    check_items_read(n,1,__LINE__,__FILE__);
    n = fread(&vectorSize, sizeof(int), 1, fInput);
    check_items_read(n,1,__LINE__,__FILE__);
    n=checkNNZ(fInput, vectorSize);
    matrix=spmat_allocate(vectorSize, n);

    /*initialize row and*/
    matrixRow = calloc(vectorSize,sizeof(double));
    check_allocation(matrixRow,__LINE__,__FILE__);

    /*Read matrix row from file and add to sprase matrix*/
    for(i = 0; i < vectorSize; ++i){
        /*read line in cov matrix*/
        n=fread(matrixRow, sizeof(double), vectorSize, fInput);
        check_items_read(n,vectorSize,__LINE__,__FILE__);
        /*add_row_sparse(matrix,matrixRow,i);*/
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
    check_open_file(fOutput, location ,__LINE__,__FILE__ );

    /*initialize row and*/
    eigenVector = calloc(matrix->n,sizeof(double));
    check_allocation(eigenVector,__LINE__,__FILE__);

    /*calc eigen with power iteration*/
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

/*print regular adjacency matrix in file location*/
void printFileMatrix(char *location){
    FILE *fInput;
    int vectorLen,VectorWid, n, i,j;
    double *matrixRow;

    /* open files of matrix*/
    fInput = fopen(location, "r");
    check_open_file(fInput, location, __LINE__,__FILE__);

    /*initialize row and*/
    matrixRow = calloc(VectorWid,sizeof(double));
    check_allocation(matrixRow,__LINE__,__FILE__);

    /*initialize matrix with size from the file*/
    n = fread(&vectorLen, sizeof(int), 1, fInput);
    check_items_read(n,1,__LINE__,__FILE__);
    n = fread(&VectorWid, sizeof(int), 1, fInput);
    check_items_read(n,1,__LINE__,__FILE__);
    printf("The matrix is %d x %d\n",vectorLen,VectorWid);

    /*Read matrix row from file and add to sprase matrix*/
    for(i = 0; i < vectorLen; ++i){
        /*read line in cov matrix*/
        n=fread(matrixRow, sizeof(double), VectorWid, fInput);
        check_items_read(n,VectorWid,__LINE__,__FILE__);
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
void createMatrixFile(int *values,int n, int m, char *location){
    FILE *fOutput;
    int check,i;

    fOutput = fopen(location, "w");
    check_open_file(fOutput, location ,__LINE__,__FILE__ );

    /* check =fwrite(&m, sizeof(int), 1, fOutput);
     assert(check==1);
     check =fwrite(&n, sizeof(int), 1, fOutput);
     assert(check==1);*/
    for(i=0;i<m;i++){
        check = fwrite(values, sizeof(int), n, fOutput);
        assert(check==n);
        values+=n;
    }
    fclose(fOutput);
}

/* print received vector*/
void printVectorInt(int *vector, int vectorSize){
    int i;
    for(i=0;i<vectorSize;i++){
        printf("%d, ",vector[i]);
    }
    printf("\n");
}

/* print received vector*/
void printVectorDouble(double *vector, int vectorSize){
    int i;
    for(i=0;i<vectorSize;i++){
        printf("%f, ",vector[i]);
    }
    printf("\n");
}

void printAllIntValuesFromFIle(char *location){
    int value,i=0;
    FILE *fInput;
    fInput = fopen(location, "r");
    check_open_file(fInput, location ,__LINE__,__FILE__ );
    while(!feof(fInput)){
        fread(&value, sizeof(int), 1, fInput);
        printf("%d (i=%d), ",value,i);
        i++;
    }
    printf("\n");
    fclose(fInput);
}

void printFinalGroups(char *location){
    int value,i,j, numOfGroups, numInGroup_i;
    FILE *fInput;
    fInput = fopen(location, "r");
    check_open_file(fInput, location ,__LINE__,__FILE__ );
    fread(&numOfGroups, sizeof(int), 1, fInput);
    printf("There is %d groups in division\n",numOfGroups);
    for (i = 0;  i< numOfGroups; i++) {
        fread(&numInGroup_i, sizeof(int), 1, fInput);
        printf("Group %d: { ",i);
        for (j = 0; j < numInGroup_i-1 ; ++j) {
            fread(&value, sizeof(int), 1, fInput);
            printf("%d , ",value);
        }
        fread(&value, sizeof(int), 1, fInput);
        printf("%d }\n",value);
    }
    printf("\n");
    fclose(fInput);
}

/* test all the graft as given by Moshe*/
void test10(){
    char *locationInput="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\graph20.in";
    char *locationOutput="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\outputs\\groups20.in";
    char *arr[3];
    arr[0]="sonia and nir are the best!";
    arr[1]=locationInput;
    arr[2]=locationOutput;
    cluster(10,arr);
    printFinalGroups(locationOutput);
};

/* test specific graph on NOVA*/
void test11(){
    char *locationInput="inputs/graph10000.input";
    char *locationOutput="outputs/graph10000.output";
    char *arr[3];
    arr[0]="sonia and nir are the best!";
    arr[1]=locationInput;
    arr[2]=locationOutput;
    cluster(10,arr);
    printFinalGroups(locationOutput);
}

/* test specific graph with valgrid*/
void test12(){
    char *locationInput="tester_binary.input";
    /*char *locationInput="graph20.in";*/
    char *locationOutput="tester_binary.output";
    char *arr[3];
    arr[0]="sonia and nir are the best!";
    arr[1]=locationInput;
    arr[2]=locationOutput;
    cluster(10,arr);
    printFinalGroups(locationOutput);
}
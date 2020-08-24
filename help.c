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
void createMatrixFile(int *values,int n, int m, char *location){
    FILE *fOutput;
    int check,i;

    fOutput = fopen(location, "w");
    checkOpenFile(fOutput, location ,__LINE__,__FILE__ );

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
    checkOpenFile(fInput, location ,__LINE__,__FILE__ );
    while(!feof(fInput)){
        fread(&value, sizeof(int), 1, fInput);
        printf("%d (i=%d), ",value,i);
        i++;
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

    int values[18]={5,2,1,2,3,0,2,3,3,0,1,4,2,1,4,2,2,3};
    char *location="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\graph5.in";
    createMatrixFile(values,18,1,location);
    printAllIntValuesFromFIle(location);
    /*printFileMatrix(location);*/
}

void test3() {
    char *f1 = "C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\graph5.in";
    modMat *myMat;
    myMat = modMat_allocate(f1);
}

/*tester of linkedList*/
void test4(){
    /*linkedList *l1=linkedList_allocate();
    int* arr1[5]={1,2,3,4,5};
    int* arr2[4]={10,20,30,40};
    int* arr3[2]={1000,2000};
    int* arr4[0]={};
    int* arr5=calloc(3,sizeof(int));
    arr5[0]=-1;
    arr5[1]=-2;
    arr5[2]=-3;
    int* ar1,*ar2, s1, s2;
    l1->insert_first(l1,arr1,5);
    l1->insert_first(l1,arr2,4);
    l1->insert_last(l1,arr3,2);
    l1->insert_last(l1,arr4,0);
    l1->insert_last(l1,arr5,3);
    l1->printList(l1);
    printf("\n");

    printVectorInt(l1->head->data,l1->head->size);
    printf("\n");

    l1->delete_first(l1,0);
    l1->printList(l1);
    printf("\n");

    ar1=l1->head->data;
    s1=l1->head->size;
    printVectorInt(ar1,s1);
    l1->delete_first(l1,1);
    printVectorInt(ar1,s1);
    l1->printList(l1);
    printf("\n");

    ar2=l1->tail->data;
    s2=l1->tail->size;
    printVectorInt(ar2,s2);
    l1->delete_last(l1,0);
    printVectorInt(ar2,s2);
    l1->printList(l1);
    printf("\n");

    l1->delete_last(l1,0);
    l1->printList(l1);
    printf("\n");

    l1->delete_last(l1,1);
    printf("done\n");
    l1->printList(l1);
*/
}
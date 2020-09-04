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
    checkOpenFile(fInput, location, __LINE__,__FILE__);

    /*initialize matrix with size from the file*/
    n = fread(&vectorSize, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__,__FILE__);
    n = fread(&vectorSize, sizeof(int), 1, fInput);
    checkItemsRead(n,1,__LINE__,__FILE__);
    n=checkNNZ(fInput, vectorSize);
    matrix=spmat_allocate(vectorSize, n);

    /*initialize row and*/
    matrixRow = calloc(vectorSize,sizeof(double));
    checkAllocation(matrixRow,__LINE__,__FILE__);

    /*Read matrix row from file and add to sprase matrix*/
    for(i = 0; i < vectorSize; ++i){
        /*read line in cov matrix*/
        n=fread(matrixRow, sizeof(double), vectorSize, fInput);
        checkItemsRead(n,vectorSize,__LINE__,__FILE__);
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
    checkOpenFile(fOutput, location ,__LINE__,__FILE__ );

    /*initialize row and*/
    eigenVector = calloc(matrix->n,sizeof(double));
    checkAllocation(eigenVector,__LINE__,__FILE__);

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
    fclose(fInput);
}

void printFinalGroups(char *location){
    int value,i,j, numOfGroups, numInGroup_i;
    FILE *fInput;
   /* printf("g is { ");
    for (i = 0;  i<gLen-1 ; i++) {
        printf("%d, ", g[i]);
    }
    printf("%d }\n", g[i]);*/
    fInput = fopen(location, "r");
    checkOpenFile(fInput, location ,__LINE__,__FILE__ );
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

/*tester of power iteration*/
void test1(){
    char *inputL="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\input5.arr";
    char *outputL="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\outputs\\output5.arr";

    printFileMatrix(inputL);
    printf("\n");

    spmat *matrix=fileToSparseMatrix(inputL);
   /* matrix->printSparse(matrix);*/
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

/*tester of new sparse mult: A[g]*b */
void test5(){
    int i;
    double arr[5][5] = {{0,1,1,0,0},{1,0,1,1,0},{1,1,0,0,1},{0,1,0,0,1},{0,0,1,1,0}};
    double v[3] = {5,0,2};
    int g[3]={1,2,4};
    double res[3];
    spmat *mys = NULL;

    mys=spmat_allocate(5,12);
    for (i = 0; i < 5; ++i) {
        /*add_row_sparse(mys, arr[i], i);*/
    }

    mult_sparse(mys,v,res,g,3);
    printVectorDouble(res,3);
}

void test6(){
    char *f1 = "C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\graph5.in";
    modMat *myMat;
    double norm;
    int g[3]={1,2,4};
    printf("hi\n");
    myMat = modMat_allocate(f1);
    printB(myMat);
    /*norm = normCalc(myMat, g, 3);*/
    printf("norm function is: %f", norm);
}

/*test mult b^*/
void test7(){
    char *f1 = "C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\graph5.in";
    modMat *myMat;
    double norm;
    int g[3]={1,2,4};
    double v[3]={5.0,0.0,2.0};
    double *result=calloc(3, sizeof(double));
    myMat = modMat_allocate(f1);
    printB(myMat);
    update_HatB_f_vector(myMat,g,3);
    mult_HatB(myMat,v,result,g,3);
    printVectorDouble(result,3);
}
/* make same graph as given by Moshe */
void test8(){
    int values[103]={20,2,2,4,3,2,3,4,5,0,1,4,5,6,5,1,4,5,6,11,7,0,1,2,3,5,6,7,4,2,3,4,15,3,2,3,4,5,4,8,10,11,12,5,7,10,11,13,16,3,10,12,13,6,7,8,9,11,12,14,4,3,7,8,10,4,7,9,10,14,4,8,9,14,18,3,10,12,13,4,5,16,17,19,5,8,15,17,18,19,4,15,16,18,19,3,13,16,17,3,15,16,17};
    char *location="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\graph20.in";
    createMatrixFile(values,103,1,location);
    printAllIntValuesFromFIle(location);
}

/* test partly the graft as given by Moshe*/
void test9(){
    double *eigen;
    int g[20]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19}, **division, g1[13]={7,8,9,10,11,12,13,14,15,16,17,18,19};
    char *location="C:\\Users\\Nir Swartz\\CLionProjects\\SP-Project\\inputs\\graph20.in";
    modMat *B=modMat_allocate(location);
    /*B->A->printSparse(B->A);
    printB(B);*/
    eigen=calloc(B->n, sizeof(double));
    division = calloc(4, sizeof(int*));
    checkAllocation(division,__LINE__,__FILE__);
    division[2] = calloc(1, sizeof(int));
    checkAllocation(division[2],__LINE__,__FILE__);
    division[3]=calloc(1, sizeof(int));
    checkAllocation(division[3],__LINE__,__FILE__);
    /*calcTwoDivision(B,division,g,20);
    printf("g:\n");
    printVectorInt(g,20);
    printf("g1:\n");
    printVectorInt(division[0],*division[2]);
    printf("g2:\n");
    printVectorInt(division[1],*division[3]);*/
    calc_two_division(B,division,g1,13);
    printf("g1 divided:\n");
    printf("g1:\n");
    printVectorInt(division[0],*division[2]);
    printf("g2:\n");
    printVectorInt(division[1],*division[3]);
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
    char *locationInput="inputs\\tester_binary.input";
    char *locationOutput="outputs\\tester_binary.output";
    char *arr[3];
    arr[0]="sonia and nir are the best!";
    arr[1]=locationInput;
    arr[2]=locationOutput;
    cluster(10,arr);
    printFinalGroups(locationOutput);
}
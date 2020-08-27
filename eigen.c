#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "errors.h"
#include "spmat.h"
#include "help.h"
#include "modMat.h"

double epsilon = 0.00001;

/*create b0, fill it with random values*/
void initialization(double *arr, int rowLength){
    int i;
    srand(time(0));
    for(i = 0; i < rowLength; ++i){
        *(arr)=rand();
        arr++;
    }
}

/*calculating dot product of two rows*/
double dotProduct(double* row1,double* row2,int rowLength){
    int i;
    double sum=0;
    for(i=0; i<rowLength; i++){
        sum += ((*(row1)) * (*(row2)));
        row1++;
        row2++;
    }
    return sum;
}

/* (Ab) / (||Ab||) */
void normalized(double *arr, int rowLength) {
    int i;
    double norm = sqrt(dotProduct(arr,arr,rowLength));
    checkDivideByZero(norm, __LINE__,__FILE__);
    for (i = 0; i < rowLength; i++) {
        *arr = (double) *arr / norm;
        arr++;
    }
}

/*calculating next eigenvector*/
void nextVector(modMat *B, double* b, double* bNew, int *g, int gLen){
    /* compute B^[g]*b , set result into bNew */
    B->multB_hat(B,b,bNew,g,gLen);
    normalized(bNew, gLen);
}

/* check if the change in the new vector is small enough*/
int bigDifference(double *b, double* bNew, int rowLength){
    int i;
    for(i = 0; i < rowLength; ++i){
        if(fabs(*b - *bNew) > epsilon)
            return 1;
        b++;
        bNew++;
    }
    return 0;
}

/* find leading eigenvector. result will be placed in vector */
void powerIteration(modMat *B, double *vector, int *g, int gLen)
{
    int		flag = 1, i;
    double  *b, *bNew, *tmp;

    /*initialize arr for eigenvector*/
    b = calloc(gLen,sizeof(double));
    checkAllocation(b, __LINE__,__FILE__);
    initialization(b, gLen);

    /*initialize new eigenvector*/
    bNew = calloc(gLen,sizeof(double));
    checkAllocation(bNew, __LINE__,__FILE__);

    /*start iterations - if difference bigger than epsilon continue, else stop iterating*/
    while(flag == 1)
    {
        /*calculation of b_(k+1)*/
        nextVector(B, b, bNew, g, gLen);
        /*calculate difference*/
        flag = bigDifference(b, bNew, gLen);
        /*switch pointers of b and bNew in order to avoid new calloc*/
        tmp = b;
        b = bNew;
        bNew = tmp;
    }

    /* copy eigenvector with greatest eigenvalue into output vector */
    tmp = bNew;
    for(i=0; i< gLen; i++){
        *vector = *bNew;
        vector++;
        bNew++;
    }
    bNew = tmp;

    /*free heap*/
    free(b);
    free(bNew);
}
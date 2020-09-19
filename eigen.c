#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "errors.h"
#include "modMat.h"

double epsilon = 0.00001;

/* function deceleration */
void power_iteration(modMat *B, double *vector, int *g, int gLen);
double dot_product(double* row1,double* row2,int rowLength);
void initialization(double *arr, int rowLength);
void normalized(double *arr, int rowLength);
void next_vector(modMat *B, double* b, double* bNew, int *g, int gLen);
int big_difference(double *b, double* bNew, int rowLength);
/* end of function deceleration */

/* Calculating dominant eigen vector of B^[g], result will be placed in vector */
void power_iteration(modMat *B, double *vector, int *g, int gLen){
    int	flag_big_diff = 1, i, total_iterations = 0;
    double  *b, *bNew, *tmp;

    /*initialize arr for eigenvector*/
    b = calloc(gLen,sizeof(double));
    check_allocation(b, __LINE__,__FILE__);
    /* Initialize random vector*/
    initialization(b, gLen);

    /*initialize new eigenvector*/
    bNew = calloc(gLen,sizeof(double));
    check_allocation(bNew, __LINE__,__FILE__);

    /*start iterations - if difference bigger than epsilon continue, else stop iterating*/
    while(flag_big_diff)
    {
        /*calculation of b_(k+1)*/
        next_vector(B, b, bNew, g, gLen);

        /*calculate difference*/
        flag_big_diff = big_difference(b, bNew, gLen);

        /*switch pointers of b and bNew in order to avoid new calloc*/
        tmp = b;
        b = bNew;
        bNew = tmp;

        total_iterations++;
        check_infinite_loop(total_iterations, B->upper_bound, __LINE__, __FILE__);
    }

    /* Copy eigenvector with greatest eigenvalue into output vector */
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

/*calculating dot product of two vectors*/
double dot_product(double* row1,double* row2,int rowLength){
    int i;
    double sum=0;
    for(i=0; i<rowLength; i++){
        sum += ((*(row1)) * (*(row2)));
        row1++;
        row2++;
    }
    return sum;
}

/*create b0, fill it with random values*/
void initialization(double *arr, int rowLength){
    int i;
    srand(time(0));
    for(i = 0; i < rowLength; ++i){
        *arr=rand();
        arr++;
    }
}

/* (Ab) / (||Ab||) */
void normalized(double *arr, int rowLength) {
    int i;
    double norm = sqrt(dot_product(arr,arr,rowLength));
    check_divide_by_zero(norm, __LINE__,__FILE__);
    for (i = 0; i < rowLength; i++) {
        *arr = *arr / (double) norm;
        arr++;
    }
}

/* Compute B^[g]*b , set result into bNew */
void next_vector(modMat *B, double* b, double* bNew, int *g, int gLen){
    mult_HatB(B,b,bNew,g,gLen);
    normalized(bNew, gLen);
}

/* Check if the change in the new vector is small enough
 * if the difference is bigger than epsilon, the output is 1
 * Otherwise the output is 0 - meaning we found the approximate eigenvector*/
int big_difference(double *b, double* bNew, int rowLength){
    int i;
    for(i = 0; i < rowLength; ++i){
        if(fabs(*b - *bNew) > epsilon)
            return 1;
        b++;
        bNew++;
    }
    return 0;
}
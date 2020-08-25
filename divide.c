#include <stdio.h>
#include <stdlib.h>
#include "eigen.h"
#include "modMat.h"
#include "errors.h"
#include "maximization.h"

void createS(double *eigenvector, int *s, int vectorSize){
    int i;
    for (i = 0; i < vectorSize; ++i) {
        if(*eigenvector > 0){
            *s = 1;
        }
        else{
            *s = -1;
        }
        eigenvector++;
        s++;
    }
}

double computeEigenvalue(modMat *B ,double *eigenvector){

    return 0.0;
}

double computeDeltaQ(modMat *B, int *s, int *g){
    return 0.0;
}

/*Divide the vertices in g into two groups g1,g2 according to s where division[0]=g1 and division[1]=g2*/
void makeDivision(int *division,int *s, int *g){

}

/*Divide a group g into two groups g1,g2 like in Algorithm 2*/
int* calcTwoDivision(modMat *B, int *arr, int *g){
    double *eigenvector, lambda, Q;
    int *s, division[2];

    eigenvector = calloc(B->n, sizeof(double));
    checkAllocation(eigenvector, __LINE__,__FILE__);

    /* stage 1 - compute leading eigenvector */
    powerIteration(B, eigenvector, g);
    lambda = computeEigenvalue(B,eigenvector); /*ìéöåø ôåð÷öéä */

    /* stage 2 */
    if(lambda <= 0){
        /*return NULL;*/
    }
    /* stage 3 - compute s */
    s = calloc(B->n, sizeof(int));
    createS(eigenvector, s, B->n);

    /* stage 4 - compute s^T*B_hat[g]*s */
    Q = computeDeltaQ(B, s, g);
    if (Q <= 0){
        /*return NULL;*/
    }

    maxDivision(B, s, g);

    /* stage 5 - divide s into g_1 and g_2 */
    makeDivision(division,s, g);

    return division;
}
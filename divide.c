#include <stdio.h>
#include <stdlib.h>
#include "eigen.h"
#include "modMat.h"
#include "errors.h"
#include "maximization.h"

#define IS_POSITIVE(X)((X)>0.00001)

void createS(double *eigenvector, int *s, int vectorSize){
    int i;
    for (i = 0; i < vectorSize; ++i) {
        if(IS_POSITIVE(*eigenvector)){
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

/*Divide the vertices in g into two groups g1,g2 according to s where division[0]=g1 and division[1]=g2
 *division[2]=size(g1) and division[3]=size(g2)*/
void makeDivision(int **division,int *s, int *g){

}

/* Divide a group g into two groups g1,g2 like in Algorithm 2
 * The result is an array division[2] where division[0]=g1 and division[1]=g2
 * division[2]=size(g1) and division[3]=size(g2)*/
void calcTwoDivision(modMat *B, int **division, int *g){
    double *eigenvector, lambda, Q;
    int *s;

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

}
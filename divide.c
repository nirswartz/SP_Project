#include <stdio.h>
#include <stdlib.h>
#include "eigen.h"
#include "modMat.h"
#include "errors.h"
#include "maximization.h"
#include "divide.h"


#define IS_POSITIVE(X)((X)>0.00001)

void createS(double *eigenvector, double *s, int vectorSize){
    int i;
    for (i = 0; i < vectorSize; ++i) {
        if(IS_POSITIVE(*eigenvector)){
            *s = 1.0;
        }
        else{
            *s = -1.0;
        }
        eigenvector++;
        s++;
    }
}

/* lambda = (eigenvector^T * B_hat[g]*eigenvector) / (eigenvector^T*eigenvector) */
double computeEigenvalue(modMat *B ,double *eigenvector, int *g, int gLen){
    double *v, dot1, dot2;
    v = calloc(gLen, sizeof(double));
    checkAllocation(v, __LINE__,__FILE__);
    /* compute B_hat[g]*eigenvector */
    B->multB_hat(B, eigenvector,v, g, gLen); /*$$$$$$$$$$$$$$$$$$$$$$$ changed mult*/
    /* compute (eigenvector^T * B_hat[g]*eigenvector) */
    dot1 = dotProduct(eigenvector,v,gLen);
    /* compute (eigenvector^T * eigenvector) */
    dot2 = dotProduct(eigenvector,eigenvector,gLen);
    checkDivideByZero(dot2, __LINE__,__FILE__);

    return (dot1 / dot2);
}
/*compute modularity Q = s^T*B^[g]*s
double computeModularity(modMat *B, double *s, int *g, int gLen) {
    double *v = calloc(gLen, sizeof(double)), result;
    checkAllocation(v, __LINE__, __FILE__);
    compute B^[g]*s
    B->multB_hat_noShift(B, s, v, g, gLen); $$$$$$$$$$$$$$$$$$$$$$$ changed mult
    compute s^T * B^[g]*s
    result = dotProduct(s, v, gLen);
    free(v);
    return result;
}*/

/*compute modularity Q = s^T*B^[g]*s according to linear algebra calculation*/
double computeModularity(modMat *B, double *s, int *g, int gLen){
    int i,j;
    double sum;
    for(i=0; i<gLen; i++){
        for (j = 0; j <gLen ; ++j) {
            if(s[i]!=s[j]){
                sum+=(-2*B->getB(B,g[i],g[j]));
            }
        }
    }
    return sum;
}

/*compute modularity Q = s^T*B^[g]*s according to linear algebra*/
/*double computeModularity(modMat *B, double *s, int *g, int gLen){
    int i,j,k;
    double sum, innerSum, sub;
    for(i=0; i<gLen; i++){
        for (j = 0; j <gLen ; ++j) {
            sub=B->getB(B,g[i],g[j]);
            innerSum=0;
            if(g[i]==g[j]){
                for (k = 0; k < gLen ; ++k) {
                    innerSum+=B->getB(B,g[i],g[k]);
                }
            }
            sub-=innerSum;
            sum+=(sub*s[i]*s[j]);
        }
    }
    printf("modularity is %f\n",sum);
    return sum;
}*/

/* couldn't find a good division of g, return g (in division[0])
 * and an empty group (in division[2]) */
void gIsIndivisible(int **division, int *g, int gLen){
    division[0] = g;
    division[1] = calloc(1, sizeof(int)); /*&&&&&&&&&& check it &&&&&&&&&&&&&*/
    checkAllocation(division[1],__LINE__,__FILE__);
    *division[2] = gLen;
    *division[3] = 0;
}

/*Divide the vertices in g into two groups g1,g2 according to s where division[0]=g1 and division[1]=g2
 *division[2]=size(g1) and division[3]=size(g2)*/
void makeDivision(int **division, double *s, int *g, int gLen, int numOfPositive){
    int *g1 , *g2, *p1, *p2, i;
    if(numOfPositive == gLen || numOfPositive == 0){
        gIsIndivisible(division, g, gLen);
        return;
    }
    g1 = calloc(numOfPositive, sizeof(int));
    checkAllocation(g1, __LINE__,__FILE__);
    g2 = calloc((gLen-numOfPositive), sizeof(int));
    checkAllocation(g2, __LINE__,__FILE__);

    p1 = g1;
    p2 = g2;
    for (i = 0; i < gLen; ++i) {
        if(*s == 1.0){
            *g1 = *g;
            g1++;
        }
        else{ /* s == -1.0 */
            *g2 = *g;
            g2++;
        }
        g++;
        s++;
    }

    division[0] = p1;
    division[1] = p2;
    *division[2] = numOfPositive;
    *division[3] = (gLen-numOfPositive);
    /*free(g);*/
}

int countPositiveValues(double *vector,int vectorSize){
    int counter = 0,i;
    for (i = 0; i < vectorSize; ++i) {
        if (IS_POSITIVE(*vector)) {
            counter++;
        }
        vector++;
    }
    return counter;
}

/* Divide a group g into two groups g1,g2 like in Algorithm 2
 * The result is an array division[4] where division[0]=g1 and division[1]=g2
 * division[2]=size(g1) and division[3]=size(g2)*/
void calcTwoDivision(modMat *B, int **division, int *g, int gLen){
    double *eigenvector, *s, lambda, Q;

    eigenvector = calloc(gLen, sizeof(double));
    checkAllocation(eigenvector, __LINE__,__FILE__);

    /*Update f vector and norm of new B^[g] before multiplication in power iteration*/
    B->updateB_Hat(B,g,gLen);

    /* stage 1 - compute leading eigenvector */
    powerIteration(B, eigenvector, g, gLen);
    lambda = computeEigenvalue(B,eigenvector,g,gLen) - (B->norm);

    /* stage 2 */
    if(lambda <= 0){
        gIsIndivisible(division, g, gLen);
        free(eigenvector);
        return;
    }

    /* stage 3 - compute s */
    s = calloc(gLen, sizeof(double));
    createS(eigenvector, s, gLen);

    /*Maximize modularity of division of vector s*/
    maxDivision(B, s, g, gLen);

    /* stage 4 - compute s^T*B_hat[g]*s */

    Q = computeModularity(B, s, g, gLen);
    if (Q <= 0){
        gIsIndivisible(division, g, gLen);
        free(eigenvector);
        free(s);
        return;
    }

    /* stage 5 - divide s into g_1 and g_2 */
    makeDivision(division,s,g,gLen,countPositiveValues(s,gLen));

    free(eigenvector);
    free(s);
}

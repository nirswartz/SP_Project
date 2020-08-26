#include <stdio.h>
#include <stdlib.h>
#include "eigen.h"
#include "modMat.h"
#include "errors.h"
#include "maximization.h"

#define IS_POSITIVE(X)((X)>0.00001)

int createS(double *eigenvector, int *s, int vectorSize){
    int i, numOfPositive = 0;
    for (i = 0; i < vectorSize; ++i) {
        if(IS_POSITIVE(*eigenvector)){
            *s = 1;
            numOfPositive++;
        }
        else{
            *s = -1;
        }
        eigenvector++;
        s++;
    }
    return numOfPositive;
}

/* lambda = (eigenvector^T * B_hat[g]*eigenvector) / (eigenvector^T*eigenvector) */
double computeEigenvalue(modMat *B ,double *eigenvector, int *g, int gLen){
    double *v, dot1, dot2;
    v = calloc(gLen, sizeof(double));
    checkAllocation(v, __LINE__,__FILE__);
    /* compute B_hat[g]*eigenvector */
    B->multB_hat(B, eigenvector,v, g, gLen);
    /* compute (eigenvector^T * B_hat[g]*eigenvector) */
    dot1 = dotProduct(eigenvector,v,gLen);
    /* compute (eigenvector^T * eigenvector) */
    dot2 = dotProduct(eigenvector,eigenvector,gLen);
    checkDivideByZero(dot2, __LINE__,__FILE__);

    return (dot1 / dot2);
}
/*compute modularity Q = s^T*B^[g]*s */
double computeDeltaQ(modMat *B, int *s, int *g, int gLen){
	double *v = calloc(gLen, sizeof(double));
	checkAllocation(v, __LINE__,__FILE__);
	/*compute B^[g]*s */
	multB_hat(B, s, v, g, gLen);
	/*compute s^T * B^[g]*s */
	return dotProduct(s, v, gLen);
}

/*Divide the vertices in g into two groups g1,g2 according to s where division[0]=g1 and division[1]=g2
 *division[2]=size(g1) and division[3]=size(g2)*/
void makeDivision(int **division,int *s, int *g, int gLen, int numOfPositive){
    double *g1 , *g2, *p1, *p2;
    int i;
    g1 = calloc(numOfPositive, sizeof(double));
    checkAllocation(g1, __LINE__,__FILE__);
    g2 = calloc((gLen-numOfPositive), sizeof(double));
    checkAllocation(g2, __LINE__,__FILE__);

    p1 = g1;
    p2 = g2;
    for (i = 0; i < gLen; ++i) {
		if(*s == 1){
			*g1 = *g;
			g1++;
		}
		else{ /* s == -1 */
			*g2 = *g;
			g2++;
		}
		g++;
		s++;
	}

	division[0] = p1;
	division[1] = p2;
	division[2] = numOfPositive;
	division[3] = (gLen-numOfPositive);
}

/* couldn't find a good division of g, return g (in division[0])
 * and an empty group (in division[2]) */
void gIsIndivisible(int **division, int *g, int gLen){
	division[0] = g;
	division[1] = calloc(0, sizeof(int));
	checkAllocation(division[1],__LINE__,__FILE__);
	division[2] = gLen;
	division[3] = 0;
}

/* Divide a group g into two groups g1,g2 like in Algorithm 2
 * The result is an array division[4] where division[0]=g1 and division[1]=g2
 * division[2]=size(g1) and division[3]=size(g2)*/
void calcTwoDivision(modMat *B, int **division, int *g, int gLen){
    double *eigenvector, lambda, Q;
    int *s, numOfPositive;

    eigenvector = calloc(gLen, sizeof(double));
    checkAllocation(eigenvector, __LINE__,__FILE__);

    /*Update f vector and norm of new B^[g] before multiplication in power iteration*/
    B->updateB_Hat(B,g,gLen);

    /* stage 1 - compute leading eigenvector */
    powerIteration(B, eigenvector, g, gLen);
    lambda = computeEigenvalue(B,eigenvector,g,gLen) - (B->last_norm);

    /* stage 2 */
    if(lambda <= 0){
    	gIsIndivisible(division, g, gLen);
    }
    /* stage 3 - compute s */
    s = calloc(gLen, sizeof(int));
    numOfPositive = createS(eigenvector, s, gLen);

    /* stage 4 - compute s^T*B_hat[g]*s */
    Q = computeDeltaQ(B, s, g, gLen);
    if (Q <= 0){
    	gIsIndivisible(division, g, gLen);
    }

    maxDivision(B, s, g); /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */

    /* stage 5 - divide s into g_1 and g_2 */
    makeDivision(division,s,g,gLen,numOfPositive);

}

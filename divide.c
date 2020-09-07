#include <stdio.h>
#include <stdlib.h>
#include "eigen.h"
#include "modMat.h"
#include "errors.h"
#include "maximization.h"
#include "divide.h"

#define IS_POSITIVE(X)((X)>0.00001)

/* functions deceleration */
void calc_two_division(modMat *B, int **division, int *g, int gLen);
double compute_modularity(modMat *B, double *s, int *g, int gLen);
void create_s(double *eigenvector, double *s, int vectorSize);
double compute_eigen_value(modMat *B ,double *eigenvector, int *g, int gLen);
void g_is_indivisible(int **division, int *g, int gLen);
void make_division(int **division, double *s, int *g, int gLen, int numOfPositive);
int count_positive_values(double *vector,int vectorSize);
/* end of functions deceleration */

/* Divide a group g into two groups g1,g2 like in Algorithm 2
 * The result is an array division[4] where division[0]=g1 and division[1]=g2
 * division[2]=size(g1) and division[3]=size(g2)*/
void calc_two_division(modMat *B, int **division, int *g, int gLen){
    double *eigenvector, *s, lambda, Q, *sStart;
    int i;

    eigenvector = calloc(gLen, sizeof(double));
    checkAllocation(eigenvector, __LINE__,__FILE__);

    /*Update f vector of new B^[g] before multiplication in power iteration*/
    update_HatB_vectors(B,g,gLen);

    /* compute leading eigenvector */
    power_iteration(B, eigenvector, g, gLen);
    lambda = compute_eigen_value(B,eigenvector,g,gLen) - (B->norm);
    /*printVectorDouble(eigenvector,gLen);
    printf("lambda is %f\n",lambda);
     deleteeeeeeeeeeeeeeeeee*/

    s = calloc(gLen, sizeof(double));
    checkAllocation(s, __LINE__,__FILE__);
    sStart = s;

    if(IS_POSITIVE(lambda)){
        /* compute s */
        create_s(eigenvector, s, gLen);
        /*compute s^T*B_hat[g]*s */
        Q = compute_modularity(B, s, g, gLen);

        if(!IS_POSITIVE(Q)){
            /* compute s */
            for (i = 0; i < gLen; ++i) {
                *s = 1;
                s++;
            }
            s = sStart;
        }
        /*Maximize modularity of division of vector s*/
        max_division(B, s, g, gLen);
        /*compute s^T*B_hat[g]*s */
        Q = compute_modularity(B, s, g, gLen);
        if (IS_POSITIVE(Q)){
            /* divide s into g_1 and g_2 */
            make_division(division,s,g,gLen,count_positive_values(s,gLen));
            free(eigenvector);
            free(s);
            return;
        }
    }
    g_is_indivisible(division, g, gLen);
    free(eigenvector);
    free(s);
}

/*compute modularity Q = s^T*B^[g]*s according to linear algebra calculation*/
double compute_modularity(modMat *B, double *s, int *g, int gLen){
    int i,j;
    double sum=0;
    for(i=0; i<gLen; i++){
        for (j = 0; j <gLen ; ++j) {
            if(s[i]!=s[j]){
                sum-=(getter_B(B,g[i],g[j]));
            }
        }
    }
    return sum;
}

void create_s(double *eigenvector, double *s, int vectorSize){
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
double compute_eigen_value(modMat *B ,double *eigenvector, int *g, int gLen){
    double *v, dot1, dot2;
    v = calloc(gLen, sizeof(double));
    checkAllocation(v, __LINE__,__FILE__);
    /* compute B_hat[g]*eigenvector */
    mult_HatB(B, eigenvector,v, g, gLen);
    /* compute (eigenvector^T * B_hat[g]*eigenvector) */
    dot1 = dot_product(eigenvector,v,gLen);
    /* compute (eigenvector^T * eigenvector) */
    dot2 = dot_product(eigenvector,eigenvector,gLen);
    checkDivideByZero(dot2, __LINE__,__FILE__);
    free(v);
    return (dot1 / dot2);
}

/*deleteeeeeeeeeeeeeeeee*/
/*compute modularity Q = s^T*B^[g]*s according to linear algebra
double computeModularity(modMat *B, double *s, int *g, int gLen){
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
	return sum;
}*/

/* g is indivisible, return g in division[0]
 * and an empty group in division[2] */
void g_is_indivisible(int **division, int *g, int gLen){
    division[0] = g;
    division[1] = calloc(1, sizeof(int));
    checkAllocation(division[1],__LINE__,__FILE__);
    *division[2] = gLen;
    *division[3] = 0;
}

/*Divide the vertices in g into two groups g1,g2 according to s where division[0]=g1 and division[1]=g2
 *division[2]=size(g1) and division[3]=size(g2)*/
void make_division(int **division, double *s, int *g, int gLen, int numOfPositive){
    int *g1 , *g2, *p1, *p2, i, *gStart;
    if(numOfPositive == gLen || numOfPositive == 0){
        g_is_indivisible(division, g, gLen);
        return;
    }
    g1 = calloc(numOfPositive, sizeof(int));
    checkAllocation(g1, __LINE__,__FILE__);
    g2 = calloc((gLen-numOfPositive), sizeof(int));
    checkAllocation(g2, __LINE__,__FILE__);

    gStart = g;
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
    g = gStart;
    free(g);
}

/* Count how many positive values are in vector*/
int count_positive_values(double *vector,int vectorSize){
    int counter = 0,i;
    for (i = 0; i < vectorSize; ++i) {
        if (IS_POSITIVE(*vector)) {
            counter++;
        }
        vector++;
    }
    return counter;
}


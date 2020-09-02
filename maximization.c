#include <stdio.h>
#include <stdlib.h>
#include "maximization.h"
#include "modMat.h"
#include "errors.h"

#define IS_POSITIVE(X)((X)>0.00001)
#define MAX_NEGATIVE_DOUBLE -2147483647.0

/*functions deceleration*/
void max_division(modMat *B, double *s, int *g, int gLen);
double calc_score(modMat *B, double *s, int *g, int gLen, int i);
void initialVector(int *vector, int len);
/*end of functions deceleration*/

/*Maximize the division from Algorithm 2 like in Algorithm 4*/
void max_division(modMat *B, double *s, int *g, int gLen) {
    double deltaQ, score, improve, max_score, max_improve;
    int *indices, *unmoved, i, k, maxIndexInScore = 0, maxIndexInImprove = 0;

    indices = calloc(gLen, sizeof(int));
    checkAllocation(indices, __LINE__, __FILE__);

    /*Create Unmoved*/
    unmoved = malloc(gLen * sizeof(int));
    checkAllocation(unmoved, __LINE__, __FILE__);

    do {
        /*trying to find an improvement of the partition defined by s*/
        initialVector(unmoved,gLen);
        max_improve=MAX_NEGATIVE_DOUBLE;
        for (i = 0; i < gLen; ++i) {
            /* lines 4 - 10: Computing deltaQ for the move of each unmoved vertex*/
            max_score=MAX_NEGATIVE_DOUBLE;
            for (k = 0; k < gLen; ++k) {
                if (unmoved[k] == 0) {
                    s[k] *= -1;
                    score = calc_score(B,s,g,gLen,k);
                    /*compute max{score[j] : j in Unmoved}*/
                    if (score > max_score) {
                        max_score = score;
                        maxIndexInScore = k;
                    }
                    s[k] *= -1;
                }
            }

            /* lines 11 - 20: Moving vertex j' with a maximal score*/
            s[maxIndexInScore] *= -1;
            indices[i] = maxIndexInScore;
            if (i == 0) {
                improve = max_score;
            } else {
                improve += max_score;
            }
            if (improve >= max_improve) {
                maxIndexInImprove = i;
                max_improve = improve;
            }
            unmoved[maxIndexInScore] = 1;
        }

        /* lines 21 - 30: find the maximum improvement of s and update s accordingly*/
        for (i = gLen - 1; i > maxIndexInImprove; i--) {
            s[indices[i]] *= -1;
        }

        if (maxIndexInImprove == gLen - 1) {
            deltaQ = 0;
        } else {
            deltaQ = max_improve;
        }
    } while (IS_POSITIVE(deltaQ));
    free(unmoved);
    free(indices);
}

/*Calc the score after moving s[k]=-s[k] according to linear algebra calculation*/
double calc_score(modMat *B, double *s, int *g, int gLen, int i){
    double sum = 0, Ag_ij, Kg_ij, k_i_divM;
    int j, g_i;
    g_i = g[i];
    k_i_divM=B->k[g_i] / (double)B->M;
    for (j = 0; j <gLen ; ++j) {
        Ag_ij = getter_sparse(B->A,g_i,g[j]);
        Kg_ij = k_i_divM * B->k[g[j]];
        sum+=((Ag_ij-Kg_ij)*s[j]);
    }
    return (4.0 * ( s[i] * sum + (k_i_divM * B->k[g_i])));
}

/*Initial Vector with zeros*/
void initialVector(int *vector, int len){
    int i;
    for (i = 0; i < len; ++i) {
        *vector = 0;
        vector++;
    }
}
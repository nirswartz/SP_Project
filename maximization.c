#include "maximization.h"
#include "modMat.h"
#include <stdio.h>
#include <stdlib.h>
#include "divide.h"
#include "errors.h"

#define IS_POSITIVE(X)((X)>0.00001)

/* make a copy of original vector into copy vector */
void initialVector(int *vector, int len){
    int i;
    for (i = 0; i < len; ++i) {
        *vector = i;
        vector++;
    }
}

/*Calc the score after moving s[k]=-s[k] according to linear algebra calculation*/
double calcScore(modMat *B, double *s, int *g, int gLen, int i){
    double sum = 0, Ag_ij, Kg_ij;
    int j, g_i;
    g_i=g[i];
    for (j = 0; j <gLen ; ++j) {
        Ag_ij = B->A->getA(B->A,g_i,g[j]);
        Kg_ij = B->k[g_i] * B->k[g[j]];
        sum+=((Ag_ij-(Kg_ij / (double)B->M))*(s[j]));
    }
    return (4.0 * ( s[i] * sum + (B->k[g_i] * B->k[g_i] / (double) B->M)));
}

/*Maximize the division from Algorithm 2 like in Algorithm 4*/
void maxDivision(modMat *B, double *s, int *g, int gLen) {
    double deltaQ, score, improve, max_score = 0, max_improve, *sStart = s;
    int *indices, *unmoved, i, k, maxIndexInScore = 0, maxIndexInImprove = 0, *unmovedStart;

    indices = calloc(gLen, sizeof(int));
    checkAllocation(indices, __LINE__, __FILE__);

    /*Create Unmoved = g = {0,1,...,n_g-1}*/
    unmoved = calloc(gLen, sizeof(int));
    checkAllocation(unmoved, __LINE__, __FILE__);
    initialVector(unmoved, gLen);
    unmovedStart = unmoved; /* pointer to the start of unmoved */

    do {
        /*trying to find an improvement of the partition defined by s*/
        for (i = 0; i < gLen; ++i) {
            /* lines 4 - 10: Computing deltaQ for the move of each unmoved vertex*/
            for (k = 0; k < gLen; ++k) {
                if (*unmoved != -1) {
                    *s = -(*s);
                    score = calcScore(B,sStart,g,gLen,k);
                    /*compute max{score[j] : j in Unmoved}*/
                    if (score > max_score) {
                        max_score = score;
                        maxIndexInScore = k;
                    }
                    *s = -(*s);
                }
                s++;
                unmoved++;
            }
            unmoved = unmovedStart;
            s = sStart;

            /* lines 11 - 20: Moving vertex j' with a maximal score*/
            s[maxIndexInScore] = -(s[maxIndexInScore]);
            *indices = maxIndexInScore;
            indices++;
            if (i == 0) {
                improve = max_score;
                max_improve = max_score;
            } else {
                improve += max_score;
                if (improve > max_improve) {
                    maxIndexInImprove = i;
                    max_improve = improve;
                }
            }
            unmoved[maxIndexInScore] = -1;
        }

        /* lines 21 - 30: find the maximum improvement of s and update s accordingly*/
        for (k = gLen - 1; k > maxIndexInImprove; k--) {
            indices--;
            s[*indices] = -s[*indices];
        }

        if (maxIndexInImprove == gLen - 1) {
            deltaQ = 0;
        } else {
            deltaQ = max_improve;
        }
    } while (IS_POSITIVE(deltaQ));
}

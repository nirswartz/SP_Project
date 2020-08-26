#include "maximization.h"
#include "modMat.h"
#include <stdio.h>
#include <stdlib.h>
#include "divide.h"
#include "errors.h"

#define IS_POSITIVE(X)((X)>0.00001)

/* make a copy of original vector into copy vector */
void copyVector(int *original, int *copy, int len){
    int i;
    for (i = 0; i < len; ++i) {
        *copy = *original;
        copy++;
        original++;
    }
}

/*Maximize the division from Algorithm 2 like in Algorithm 4*/
void maxDivision(modMat *B, double *s, int *g, int gLen){
    double deltaQ, *score, *improve, Q_0, maxValue = 0;
    int *indices, *unmoved, i, k, maxIndexInScore = 0, maxIndexInImprove = 0, *unmovedStart;

    score = calloc(gLen, sizeof(double));
    checkAllocation(score, __LINE__,__FILE__);
    improve = calloc(gLen, sizeof(double));
    checkAllocation(improve, __LINE__,__FILE__);
    indices = calloc(gLen, sizeof(int));
    checkAllocation(indices, __LINE__,__FILE__);

    /*Create Unmoved = g = {0,1,...,n_g-1}*/
    unmoved = calloc(gLen, sizeof(int));
    checkAllocation(unmoved, __LINE__,__FILE__);
    copyVector(g,unmoved,gLen);
    unmovedStart = unmoved; /* pointer to the start of unmoved */

    while(IS_POSITIVE(deltaQ)){
        /*trying to find an improvement of the partition defined by s*/
        for (i = 0; i < gLen; ++i) {
            /* lines 4 - 10: Computing deltaQ for the move of each unmoved vertex*/
            Q_0 = computeDeltaQ(B, s, g, gLen);
            for (k = 0; k < gLen; ++k) {
                if(*unmoved != -1){
                    s[k] = -s[k];
                    score[k] = computeDeltaQ(B, s, g, gLen) - Q_0;
                    /* compute max{score[j] : j in Unmoved} */
                    if(score[k] > maxValue){
                        maxValue = score[k];
                        maxIndexInScore = k;
                    }
                    s[k] = -s[k];
                }
                unmoved++;
            }
            unmoved = unmovedStart;

            /* lines 11 - 20: Moving vertex j' with a maximal score*/
            s[maxIndexInScore] = -(s[maxIndexInScore]);
            indices[i] = maxIndexInScore;
            if(i == 0){
                improve[i] = score[maxIndexInScore];
                maxValue = improve[i];
            } else{
                improve[i] = improve[i-1] + score[maxIndexInScore];
                if(improve[i] > maxValue){
                    maxIndexInImprove = i;
                    maxValue = improve[i];
                }
            }
            unmoved[maxIndexInScore] = -1;
        }

        /* lines 21 - 30: find the maximum improvement of s and update s accordingly*/
        for (k = gLen-1; k > maxIndexInImprove ; k--) {
            s[indices[k]] = -s[indices[k]];
        }

        if(maxIndexInImprove == gLen-1){
            deltaQ = 0;
        } else{
            deltaQ = improve[maxIndexInImprove];
        }
    }
}

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
/*void copyVector(int *vector, int *original, int len);*/
/*end of functions deceleration*/

/*Maximize the division from Algorithm 2 like in Algorithm 4*/
void max_division(modMat *B, double *s, int *g, int gLen) {
    double deltaQ, improve, max_score, max_improve, *scores;
    int *indices, *unmoved, i, k, maxIndexInScore = 0, maxIndexInImprove = 0 , last_max_index = 0;

    /*Create Unmoved, indices and scores*/
    indices = calloc(gLen, sizeof(int));
    check_allocation(indices, __LINE__, __FILE__);
    unmoved = malloc(gLen * sizeof(int));
    check_allocation(unmoved, __LINE__, __FILE__);
   /* last_s = malloc(gLen * sizeof(int));
    check_allocation(unmoved, __LINE__, __FILE__);*/


    /*Using calc double vector of B instead of allocate new vector*/
    scores = B->calc_double_vector;

    do {
        /*trying to find an improvement of the partition defined by s*/
        initialVector(unmoved,gLen);
        /*copyVector(last_s,s, gLen);*/
        max_improve = MAX_NEGATIVE_DOUBLE;
        for (i = 0; i < gLen; ++i) {
            /* lines 4 - 10: Computing deltaQ for the move of each unmoved vertex*/
            max_score = MAX_NEGATIVE_DOUBLE;
            if(i == 0) {
                for (k = 0; k < gLen; ++k) {
                    if (unmoved[k] == 0) {
                        /*calc the first score based and linear algebra*/
                        s[k] *= -1;
                        scores[k] = calc_score(B, s, g, gLen, k);
                        s[k] *= -1;
                        /*compute max{score[j] : j in Unmoved}*/
                        if (scores[k] > max_score) {
                            max_score = scores[k];
                            maxIndexInScore = k;
                        }
                    }
                }
            }
            else {
                for (k = 0; k < gLen; ++k) {
                    if (unmoved[k] == 0) {
                        /*calc the score based on the previous calculation and linear algebra*/
                        scores[k] -= (8.0 * s[k] * s[last_max_index] * getter_B(B,g[last_max_index],g[k]));
                        /*compute max{score[j] : j in Unmoved}*/
                        if (scores[k] > max_score) {
                            max_score = scores[k];
                            maxIndexInScore = k;
                        }
                    }
                }
            }
            last_max_index = maxIndexInScore;

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

            /*mark as moved vertex*/
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
        /*if(max_improve != 0 || max_improve != -0){
             lines 21 - 30: find the maximum improvement of s and update s accordingly
            for (i = gLen - 1; i > maxIndexInImprove; i--) {
                s[indices[i]] *= -1;
            }
            deltaQ = max_improve;
        }
        else{
            deltaQ = 0;
        }*/
    } while (IS_POSITIVE(deltaQ));
    /*copyVector(s,last_s, gLen);*/

    /*free all allocations*/
    free(unmoved);
    free(indices);
    /*free(last_s);*/
}

/*Calc the score after moving s[k]=-s[k] according to linear algebra calculation*/
double calc_score(modMat *B, double *s, int *g, int gLen, int i){
    double sum = 0;/* Ag_ij, Kg_ij, k_i_divM;*/
    int j, g_i;
    g_i = g[i];
    for (j = 0; j <gLen ; ++j) {
        sum += (getter_B(B, g_i, g[j]) * s[j]);
    }
    return  4.0 * (s[i] * sum + ((B->k[g_i] * B->k[g_i]) / (double)B->M));
}

/*Initial Vector with zeros*/
void initialVector(int *vector, int len){
    int i;
    for (i = 0; i < len; ++i) {
        *vector = 0;
        vector++;
    }
}

/* Copy the values from the original vector to other vector
void copyVector(int *vector, int *original, int len){
    int i;
    for (i = 0; i < len; ++i) {
        *vector = *original;
        vector++;
        original++;
    }
}*/
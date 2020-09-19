#include <stdio.h>
#include <stdlib.h>
#include "eigen.h"
#include "modMat.h"
#include "errors.h"
#include "maximization.h"
#include "divide.h"

#define IS_POSITIVE(X)((X)>0.00001)

/* functions deceleration */
division* division_allocate();
void free_division(division *my_division);
void calc_two_division(modMat *B, division *my_division, int *g, int gLen);
double compute_modularity(modMat *B, double *s, int *g, int gLen);
void create_s(double *eigenvector, double *s, int vectorSize);
double compute_eigen_value(modMat *B ,double *eigenvector, int *g, int gLen);
void g_is_indivisible(division *my_division, int *g, int gLen);
void make_division(division *my_division, double *s, int *g, int gLen, int numOfPositive);
int count_positive_values(double *vector,int vectorSize);
/* end of functions deceleration */

/* Allocates a new division*/
division* division_allocate(){
    division *my_division = (division*) malloc(sizeof(division));
    check_allocation(my_division, __LINE__,__FILE__);
    my_division->g1 = NULL;
    my_division->g2 = NULL;
    my_division->g1_size = 0;
    my_division->g2_size = 0;
    return my_division;
}

/* Frees all resources used by division */
void free_division(division *my_division){
    if(my_division->g1 != NULL){
        free(my_division->g1);
    }
    if(my_division->g2 != NULL){
        free(my_division->g2);
    }
}

/* Divide a group g into two groups g1,g2 like in Algorithm 2
 * The result is a division struct with the data of g1, g2, size of g1, size of g2*/
void calc_two_division(modMat *B, division *my_division, int *g, int gLen){
    double *eigenvector, *s, lambda, Q, *sStart;
    int i;

    eigenvector = calloc(gLen, sizeof(double));
    check_allocation(eigenvector, __LINE__,__FILE__);

    /* Update f vector and A[g] of new B^[g] before multiplication in power iteration*/
    update_HatB(B,g,gLen);

    /* Compute leading eigenvector */
    power_iteration(B, eigenvector, g, gLen);
    lambda = compute_eigen_value(B,eigenvector,g,gLen) - (B->norm);

    /* Allocate s vector which will hold the division
     * if s[i] = 1, the i vertex belongs to g1
     * Otherwise if s[i] = -1, it belongs to g2 */
    s = calloc(gLen, sizeof(double));
    check_allocation(s, __LINE__,__FILE__);
    sStart = s;

    if(IS_POSITIVE(lambda)){
        /* Compute vector s according to eigenvector */
        create_s(eigenvector, s, gLen);

        /*Compute the modularity (s^T*B_hat[g]*s) */
        Q = compute_modularity(B, s, g, gLen);

        if(!IS_POSITIVE(Q)){
            /* Initial vector s = {1,...,1} */
            for (i = 0; i < gLen; ++i) {
                *s = 1;
                s++;
            }
            s = sStart;
        }

        /* Maximize modularity of the division defined by vector s*/
        max_division(B, s, g, gLen);

        /* Compute the new modularity (s^T*B_hat[g]*s) */
        Q = compute_modularity(B, s, g, gLen);

        if (IS_POSITIVE(Q)){
            /* In this case g is divisible
             * divide g into g_1 and g_2 according to s */
            make_division(my_division,s,g,gLen,count_positive_values(s,gLen));
            free(eigenvector);
            free(s);
            return;
        }
    }
    /* In this case g is indivisible*/
    g_is_indivisible(my_division, g, gLen);
    free(eigenvector);
    free(s);
}

/* Compute modularity Q = s^T*B^[g]*s according to linear algebra calculation*/
double compute_modularity(modMat *B, double *s, int *g, int gLen){
    int i,j;
    double sum = 0;
    for(i=0; i < gLen; i++){
        for (j = 0; j < gLen ; ++j) {
            if(s[i] != s[j]){
                sum -= (getter_B(B,g[i],g[j]));
            }
        }
    }
    return sum;
}

/* Calculate s vector which will hold the division
 * if s[i] = 1, the i vertex belongs to g1
 * Otherwise if s[i] = -1, it belongs to g2 */
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

/* lambda = (eigenvector^T * B_hat[g] * eigenvector) / (eigenvector^T * eigenvector) */
double compute_eigen_value(modMat *B ,double *eigenvector, int *g, int gLen){
    double *v, dot1, dot2;
    v = calloc(gLen, sizeof(double));
    check_allocation(v, __LINE__,__FILE__);

    /* compute B_hat[g] * eigenvector */
    mult_HatB(B, eigenvector,v, g, gLen);

    /* compute (eigenvector^T * B_hat[g] * eigenvector) */
    dot1 = dot_product(eigenvector,v,gLen);

    /* compute (eigenvector^T * eigenvector) */
    dot2 = dot_product(eigenvector,eigenvector,gLen);

    free(v);
    check_divide_by_zero(dot2, __LINE__,__FILE__);
    return (dot1 / dot2);
}

/* g is indivisible therefore the division struct holds g in g1 and garbage in g2 */
void g_is_indivisible(division *my_division, int *g, int gLen){
    my_division->g1 = g;
    my_division->g2 = NULL;
    my_division->g1_size = gLen;
    my_division->g2_size = 0;
}

/* Divide the vertices in g into two groups g1 and g2 according to s
 * The function insert the data of g1, g2, size of g1, size of g2 into my_division in the mentioned order*/
void make_division(division *my_division, double *s, int *g, int gLen, int numOfPositive){
    int *g1 , *g2, *g1_start, *g2_start, i, *g_start;

    g1 = calloc(numOfPositive, sizeof(int));
    check_allocation(g1, __LINE__,__FILE__);
    g2 = calloc( gLen - numOfPositive, sizeof(int));
    check_allocation(g2, __LINE__,__FILE__);

    g_start = g;
    g1_start = g1;
    g2_start = g2;
    for (i = 0; i < gLen; ++i) {
        if(*s == 1.0){
            *g1 = *g;
            g1++;
        }
        else {
            /* In this case s[i] == -1.0 */
            *g2 = *g;
            g2++;
        }
        g++;
        s++;
    }

    my_division->g1 = g1_start;
    my_division->g2 = g2_start;
    my_division->g1_size = numOfPositive;
    my_division->g2_size = gLen - numOfPositive;
    g = g_start;
    free(g);
}

/* Count the number of positive values in vector*/
int count_positive_values(double *vector,int vectorSize){
    int counter = 0, i;
    for (i = 0; i < vectorSize; ++i) {
        if (IS_POSITIVE(*vector)) {
            counter++;
        }
        vector++;
    }
    return counter;
}
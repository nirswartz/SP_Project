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
    division *my_division = malloc(sizeof(division));
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

    /*Update f vector of new B^[g] before multiplication in power iteration*/
    update_HatB_vectors(B,g,gLen);

    /* compute leading eigenvector */
    power_iteration(B, eigenvector, g, gLen);
    lambda = compute_eigen_value(B,eigenvector,g,gLen) - (B->norm);

    s = calloc(gLen, sizeof(double));
    check_allocation(s, __LINE__,__FILE__);
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
            make_division(my_division,s,g,gLen,count_positive_values(s,gLen));
            free(eigenvector);
            free(s);
            return;
        }
    }
    g_is_indivisible(my_division, g, gLen);
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
    check_allocation(v, __LINE__,__FILE__);
    /* compute B_hat[g]*eigenvector */
    mult_HatB(B, eigenvector,v, g, gLen);
    /* compute (eigenvector^T * B_hat[g]*eigenvector) */
    dot1 = dot_product(eigenvector,v,gLen);
    /* compute (eigenvector^T * eigenvector) */
    dot2 = dot_product(eigenvector,eigenvector,gLen);
    free(v);
    check_divide_by_zero(dot2, __LINE__,__FILE__);
    return (dot1 / dot2);
}

/* g is indivisible, return g in my_divistion->g1
 * and an empty group in my_divistion->g2 */
void g_is_indivisible(division *my_division, int *g, int gLen){
    my_division->g1 = g;
    my_division->g2 = NULL;
    my_division->g1_size = gLen;
    my_division->g2_size = 0;
    /*TODO: division[0] = g;
    division[1] = calloc(1, sizeof(int));
    check_allocation(division[1],__LINE__,__FILE__);
    *division[2] = gLen;
    *division[3] = 0;*/
}

/*Divide the vertices in g into two groups g1,g2 according to s
 * The function insert the data of g1, g2, size of g1, size of g2 into my_division*/
void make_division(division *my_division, double *s, int *g, int gLen, int numOfPositive){
    int *g1 , *g2, *g1_start, *g2_start, i, *g_start;
    if(numOfPositive == gLen || numOfPositive == 0){
        g_is_indivisible(my_division, g, gLen);
        return;
    }
    g1 = calloc(numOfPositive, sizeof(int));
    check_allocation(g1, __LINE__,__FILE__);
    g2 = calloc((gLen-numOfPositive), sizeof(int));
    check_allocation(g2, __LINE__,__FILE__);

    g_start = g;
    g1_start = g1;
    g2_start = g2;
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
    my_division->g1 = g1_start;
    my_division->g2 = g2_start;
    my_division->g1_size = numOfPositive;
    my_division->g2_size = (gLen-numOfPositive);
    g = g_start;
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
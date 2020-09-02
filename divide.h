#ifndef SP_PROJECT_DIVIDE_H
#define SP_PROJECT_DIVIDE_H
#include "modMat.h"

/*Divide a group g into two groups g1,g2 like in Algorithm 2*/
void calc_two_division(modMat *B, int **division, int *g, int gLen);

/*compute modularity Q = s^T*B^[g]*s according to linear algebra calculation*/
double compute_modularity(modMat *B, double *s, int *g, int gLen);

#endif
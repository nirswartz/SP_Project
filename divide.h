#ifndef SP_PROJECT_DIVIDE_H
#define SP_PROJECT_DIVIDE_H
#include "modMat.h"

double computeDeltaQ(modMat *B, double *s, int *g, int gLen);

/*Divide a group g into two groups g1,g2 like in Algorithm 2*/
void calcTwoDivision(modMat *B, int **division, int *g, int gLen);

#endif
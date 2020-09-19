#ifndef SP_PROJECT_MAXIMIZATION_H
#define SP_PROJECT_MAXIMIZATION_H
#include "modMat.h"

/* Maximize the division from Algorithm 2 like in Algorithm 4
 * The algorithm updates the division vector s */
void max_division(modMat *B, double *s, int *g, int gLen);

#endif

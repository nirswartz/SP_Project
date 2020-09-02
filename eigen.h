#ifndef SP_PROJECT_EIGEN_H
#define SP_PROJECT_EIGEN_H
#include "modMat.h"

/*calculating dominant eigen vector of B^[g]*, result will be placed in vector*/
void power_iteration(modMat *B, double *vector, int *g, int gLen);

/*calculating dot product of two rows*/
double dot_product(double* row1,double* row2,int rowLength);

#endif

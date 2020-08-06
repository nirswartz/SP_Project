#ifndef SP_PROJECT_EIGEN_H
#define SP_PROJECT_EIGEN_H
#include "spmat.h"

/*create b0, fill it with random values*/
void initialization(double *arr, int rowLength);

/*calculating dot product of two rows*/
double dotProduct(double* row1,double* row2,int rowLength);

/* (Ab) / (||Ab||) */
void normalized(double* arr, double norm, int rowLength);

/*calculating next eigenvector*/
/* return the norm of bNew*/
double nextVector(FILE* fInput, double* b, double* bNew,double *matrixRow, int rowLength);

/* check if the change in the new vector is small enough*/
int bigDifference(double *b, double* bNew, int rowLength);

void powerIteration(spmat *matrix, double *vector, int vectorSize);

#endif //SP_PROJECT_EIGEN_H

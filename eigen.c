#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "errors.h"


double epsilon = 0.0001;

/*create b0, fill it with random values*/
void initialization(double *arr, int rowLength){
	int i;
	/* initialize (seed) the random number generator */
	srand(time(NULL));
	for(i = 0; i < rowLength; ++i){
        *(arr)= rand();
        arr++;
    }
}

/*calculating dot product of two rows*/
double dotProduct(double* row1,double* row2,int rowLength){
    int i;
    double sum=0;
    for(i=0; i<rowLength; i++){
        sum+=((*(row1)) * (*(row2)));
        row1++;
        row2++;
    }
    return sum;
}

/* (Ab) / (||Ab||) */
void normalized(double *arr, int rowLength) {
    int i,norm;
    norm=dotProduct(arr,arr,rowLength);
    checkDivideByZero(norm, __LINE__)
    for (i = 0; i < rowLength; i++) {
        *arr = (double) *arr / norm;
        arr++;
    }
}

/*calculating next eigenvector*/
/* return the norm of bNew*/
void nextVector(spmat *matrix, double* b, double* bNew, int rowLength){
    matrix->mult(matrix,b,bNew)
    normalized(bNew,rowLength)
}

/* check if the change in the new vector is small enough*/
int bigDifference(double *b, double* bNew, int rowLength){
	int i;
	for(i = 0; i < rowLength; ++i){
		if(fabs(*b - *bNew) > epsilon)
			return 1;
		b++;
		bNew++;
	}
	return 0;
}

void powerIteration(spmat *matrix, double *vector)
{
	int		flag=1, vectorSize=matrix->n;
	double  *b, *bNew, *tmp ,norm;

	/*initialize arr for eigenvector*/
	b = calloc(vectorSize,sizeof(double));
	checkAllocation(b, __LINE__);
	initialization(b, vectorSize);

	/*initialize new eigenvector*/
	bNew = calloc(vectorSize,sizeof(double));
    checkAllocation(bNew, __LINE__);

	/*start iterations - if difference bigger than epsilon continue, else stop iterating*/
	while(flag==1)
	{
		/*calculation of b_(k+1)*/
		nextVector(matrix, b, bNew, vectorSize);
        /*calculate difference*/
        flag=bigDifference(b, bNew, vectorSize);
        /*switch pointers of b and bNew in order to avoid new calloc*/
        tmp=b;
        b=bNew;
        bNew=tmp;
	}

	/* copy eigenvector with greatest eigenvalue into output vector */
	tmp=bNew;
    for(i=0; i<rowLength; i++){
        *vector=*bNew;
        vector++;
        bNew++;
    }
    bNew=tmp;

	/*free heap*/
	free(b);
	free(bNew);
}

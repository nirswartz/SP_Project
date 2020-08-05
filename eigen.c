/*Made by:
 * 316268853 - Nir Swartz - user: nirswartz
 * 308394113 - Sonia Moushaev - user: moushaev
 * */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>

double epsilon = 0.00001;

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
void normalized(double* arr, double norm, int rowLength){
    int i;
    for(i=0; i<rowLength; i++){
        *arr = (double)*arr / norm;
    	arr++;
    }
}

/*calculating next eigenvector*/
/* return the norm of bNew*/
double nextVector(FILE* fInput, double* b, double* bNew,double *matrixRow, int rowLength){
    int i;
	double sum=0, tmp;
	for(i = 0; i < rowLength; ++i){
		/*read line in cov matrix*/
		assert((int)(fread(matrixRow, sizeof(double), rowLength, fInput))==rowLength);
		tmp = dotProduct(matrixRow, b, rowLength);
		*(bNew) = tmp;
		sum=sum+(tmp*tmp);
		bNew++;
    }
    return sqrt(sum);
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

int main(int argc, char* argv[])
{
	int		n, vectorSize, flag=1;
	double  *b, *bNew,*matrixRow, *tmp ,norm;
	FILE	*fInput, *fOutput;

	/* open files of covariance matrix and output file*/
	fInput = fopen(argv[1], "r");
	assert(fInput != NULL);

	fOutput = fopen(argv[2], "w");
	assert(fOutput != NULL);

	/*initialize arr for eigenvector*/
	n = fread(&vectorSize, sizeof(int), 1, fInput);
    assert(n==1);
    n = fread(&vectorSize, sizeof(int), 1, fInput);
	assert(n==1);
	b = calloc(vectorSize,sizeof(double));
	assert(b!=NULL);
	initialization(b, vectorSize);

	/*initialize new eigenvector*/
	bNew = calloc(vectorSize,sizeof(double));
	assert(bNew!=NULL);

    /*initialize new matrixRaw*/
    matrixRow = calloc(vectorSize,sizeof(double));
    assert(matrixRow!=NULL);

	/*start iterations - if difference bigger than epsilon continue, else stop iterating*/
	while(flag==1)
	{
		/*calculation of b_(k+1)*/
		norm=nextVector(fInput, b, bNew, matrixRow, vectorSize);
        normalized(bNew, norm, vectorSize);
        /*calculate difference*/
        flag=bigDifference(b, bNew, vectorSize);
        /*/ file for next iteration*/
		rewind(fInput);
        assert(fread(&vectorSize, sizeof(int), 1, fInput)==1); /*just to skip matrix size*/
        assert(fread(&vectorSize, sizeof(int), 1, fInput)==1); /*just to skip matrix size*/
        /*switch pointers of b and bNew in order to avoid new calloc*/
        tmp=b;
        b=bNew;
        bNew=tmp;
	}

	/* write eigenvector with greatest eigenvalue into output file */
	flag=1;
	n =fwrite(&flag, sizeof(int), 1, fOutput);
    assert(n==1);
    n =fwrite(&vectorSize, sizeof(int), 1, fOutput);
    assert(n==1);
	n = fwrite(bNew, sizeof(double), vectorSize, fOutput);
	assert(n==vectorSize);

	/*close files*/
	fclose(fInput);
	fclose(fOutput);

	/*free heap*/
	free(b);
	free(bNew);
	free(matrixRow);

    /*final in main*/
    argc=1;
    return argc;
}


#ifndef SP_PROJECT_HELP_H
#define SP_PROJECT_HELP_H

/* calc NNZ elements of regular adjacency matrix in file location*/
int checkNNZ(FILE *fInput, int vectorSize);

/*transfer regular adjacency matrix in file location to sparse matrix*/
spmat* fileToSparseMatrix(char *location);

/*calc powerIteration of sprase matrix and write the eigen vector to location
 * vector template: (num of rows - int)(num of columns - int)(values - double) */
void calcEigenVectorAndWrite(spmat *matrix, char *location);

/*print regular adjacency matrix in file location*/
void printFileMatrix(char *location);

/* create new file of regular matrix in location
 * matrix template: (num of rows - int)(num of columns - int)(values - double)*/
void createMatrixFile(double *values,int n, int m, char *location);

/* print received vector*/
void printVector(double *vector, int vectorSize);

void printAllIntValuesFromFIle(FILE *fInput);

/*tester of power iteration*/
void test1();

/*tester of create matrix*/
void test2();

/*tester of input file to modMat*/
void test3();

#endif //SP_PROJECT_HELP_H

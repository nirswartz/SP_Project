#ifndef SP_PROJECT_HELP_H
#define SP_PROJECT_HELP_H
#include "spmat.h"

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
void createMatrixFile(int *values,int n, int m, char *location);

/* print received vector*/
void printVectorInt(int *vector, int vectorSize);

/* print received vector*/
void printVectorDouble(double *vector, int vectorSize);

void printAllIntValuesFromFIle(char *location);

void printFinalGroups(char *location);

/*tester of power iteration*/
void test1();

/*tester of create matrix*/
void test2();

/*tester of input file to modMat*/
void test3();

/*tester of linkedList*/
void test4();

/*tester of new sparse mult: A[g]*b */
void test5();

void test6();

void test7();

/* make same graph as given by Moshe */
void test8();

/* test partly the graft as given by Moshe*/
void test9();

/* test all the graft as given by Moshe*/
void test10();

#endif
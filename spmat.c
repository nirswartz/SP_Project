#include "spmat.h";
#include <stdlib.h>;

spmat* spmat_allocate_array(int n, int nnz){
	spmat *sparse = malloc(sizeof(spmat));
	sparse->n = n;
	sparse->values = (double*) malloc(nnz*sizeof(double));
	sparse->col = (int*) malloc(nnz*sizeof(int));
	sparse->row = (int*) calloc((n+1)*sizeof(int));
	return sparse;
}

void printSparse(struct _spmat *A){
	int nnz = A->row[(A->n)+1];
	printf("{");
	for (int i = 0; i < nnz; ++i) {
		printf("%d,",A->values[i]);
	}
	printf("}\n");
	printf("{");
	for (int i = 0; i < nnz; ++i) {
			printf("%d,",A->col[i]);
	}
	printf("}\n");
	printf("{");
	for (int i = 0; i < A->n; ++i) {
			printf("%d,",A->row[i]);
	}
	printf("}\n");
}

void add_row(struct _spmat *A, const double *row, int i){
	static double *valPtr = A->values, *colPtr = A->col;
	int cntNz = 0;
	double *p = row;
	for (int x = 0; x < A->n; ++x) {
		if (*p != 0){
			cntNz++;
			valPtr = *p;
			colPtr = x;
			valPtr++;
			colPtr++;
		}
		p++;
	}
	A->row[i+1] = A->row[i] + cntNz;
}

void free(struct _spmat *A){
	free(A->values);
	free(A->col);
	free(A->row);
}

void mult(const struct _spmat *A, const double *v, double *result){
	int *p1 = A->row, *p2 = p1++;
	double *valPtr = A->values, *colPtr = A->col;
	double sum;
	for (int i = 0; i < A->n; ++i) {
		sum=0;
		for (int j = 0; j < (*p2 - *p1); ++j) {
			sum += *valPtr * v[*colPtr];
			valPtr++;
			colPtr++;
		}

	}

}

void mult(const struct _spmat *A, const double *v, double *result){
	int *p1 = A->row, *p2 = p1++;
	double *valPtr = A->values, *colPtr = A->col, *res = result;
	double sum;
	for (int i = 0; i < A->n; ++i) {
		sum=0;
		for (int j = 0; j < (*p2 - *p1); ++j) {
			sum += *valPtr * v[*colPtr];
			valPtr++;
			colPtr++;
		}
		*res = sum;
		res++;
		p1++;
		p2++;
	}
}

void multRow(const struct _spmat *A, int i, const double *v, double *result){
	int numberOfElements = A->row[i+1] - A->row[i];
	int sum = 0;
	double *valPtr = A->values[(A->row[i])], *colPtr = A->col[(A->row[i])];
	for (int j = 0; j < numberOfElements; ++j) {
		sum += *valPtr * v[*colPtr];
		valPtr++;
		colPtr++;
	}
}


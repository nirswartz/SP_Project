#ifndef SP_PROJECT_DIVIDE_H
#define SP_PROJECT_DIVIDE_H
#include "modMat.h"

typedef struct _division {
    /* This struct holds the info of division Algorithm 2 - dividing group g into two groups g1, g2 */
    int* g1;
    int* g2;
    int g1_size;
    int g2_size;
} division;

/* Allocates a new division*/
division* division_allocate();

/* Frees all resources used by division */
void free_division(division *my_division);

/*Divide a group g into two groups g1,g2 like in Algorithm 2*/
void calc_two_division(modMat *B, division *my_division, int *g, int gLen);

/*compute modularity Q = s^T*B^[g]*s according to linear algebra calculation*/
double compute_modularity(modMat *B, double *s, int *g, int gLen);

#endif
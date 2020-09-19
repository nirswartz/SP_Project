#ifndef SP_PROJECT_DIVIDE_H
#define SP_PROJECT_DIVIDE_H
#include "modMat.h"

/* This module divides a given group g into two groups g1 and g2 and saves the result in division struct*/

/* This class implements a struct which holds a division of group g into two groups: g1 and g2 */
typedef struct _division {
    int* g1;
    int* g2;
    int g1_size;
    int g2_size;
} division;

/* Allocates a new division*/
division* division_allocate();

/* Frees all resources used by division */
void free_division(division *my_division);

/* Divide a group g into two groups g1,g2 like in Algorithm 2
 * The result is a division struct with the data of g1, g2, size of g1, size of g2*/
void calc_two_division(modMat *B, division *my_division, int *g, int gLen);

#endif
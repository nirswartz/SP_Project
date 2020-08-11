#include <stdio.h>
#include <stdlib.h>
#include "spmat.h"
#include "modMat.h"

int main() {
	/* main for spmat!!

    int i;
    double arr[3][3] = {{61,0,0},{0,50,0},{0,84,22}};
    double brr[3] = {1,-1,1};
    double res[3];
    spmat *mys = NULL;

    mys=spmat_allocate_array(3,3);
    for (i = 0; i < 3; ++i) {
        mys->add_row(mys, arr[i], i);
    }

    mys->mult(mys,brr,res);
    printf("{");
       for (i = 0; i < 3; ++i) {
           printf("%f,",*(res+i));
       }
       printf("}\n");
    mys->printSparse(mys);*/

	 FILE *f1;
	 f1 = fopen("graph.in","r");
	 modMat *myMat = modMat_allocate(f1);
	 printf("%d",myMat->n);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "spmat.h"
#include "modMat.h"
#include "errors.h"

int main() {

    int i, j;
    double arr[3][3] = {{61,50,0},{0,0,0},{0,84,22}};
    double brr[3] = {1,-1,1};
    double res[3];
    spmat *mys = NULL;

    mys=spmat_allocate_array(3,3);
    for (i = 0; i < 3; ++i) {
        mys->add_row(mys, arr[i], i);
    }
    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
        	printf("A[%d,%d] equals : %f\n", i,j, mys->getA(mys,i,j));
        }
    }

    mys->mult(mys,brr,res);
    printf("{");
       for (i = 0; i < 3; ++i) {
           printf("%f,",*(res+i));
       }
       printf("}\n");
    mys->printSparse(mys);


	/*FILE *f1;
	 modMat *myMat;
	 int n, *numOfNodes;

	 printf("check!!1");


	 f1 = fopen("src\\graph.in","r");
	 n = fread(&numOfNodes, sizeof(int), 1, f1);
	 printf("check!!1");
	 checkItemsRead(n,1,__LINE__);
	 printf("check!!1~");
	 myMat = modMat_allocate(f1);
	 printf("%d",myMat->n);
	 printf("check");
*/
    return 0;
}

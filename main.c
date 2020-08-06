#include <stdio.h>
#include "spmat.h"


int main() {
    int i;
    int arr[3][3] = {{61,0,0},{0,0,0},{0,84,22}};
    spmat *mys = NULL;
    mys=spmat_allocate_array(3,3);
    for (i = 0; i < 3; ++i) {
        mys->add_row(mys, arr[i], i);
    }
    mys->printSparse();
    return 0;
}

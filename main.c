#include "spmat.h";
#include <stdlib.h>;

int main(){
	int arr[3][3] = {{61,0,0},{0,0,0},{0,84,22}};
	spmat *mysparse = spmat_allocate_array(3,3);
	for (int i = 0; i < 3; ++i) {
		add_row(mysparse, arr[i], i);
	}
	printSparse(mysparse);

	return 0;

}

#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "linkedList.h"
#include "divide.h"
#include "modMat.h"

/* function deceleration */
int main(int argc, char* argv[]);
void initialization_of_groups(linkedList *P, int n);
void write_groups_to_file(linkedList *O, char *locationOutput);
/* end of function deceleration */

/* The main algorithm which calculates the clusters according to Algorithm 3*/
int main(int argc, char* argv[]){
    char *locationInput, *locationOutput;
    int *g,*g1,*g2, gLen, g1_size, g2_size;
    linkedList *O, *P;
    modMat *B;
    division *my_division;

    /* Retrieve arguments from the command line*/
    locationInput = argv[1];
    locationOutput = argv[2];

    /* Create struct which holds the division at each step*/
    my_division = division_allocate();

    /*Create matrix B from file located on locationInput*/
    B = modMat_allocate(locationInput);

    /* Stage 1+2 - initialization Of P and O*/
    P = linkedList_allocate();
    O = linkedList_allocate();
    initialization_of_groups(P, B->n);

    /* Stage 3 - calculating the modularity groups*/
    while(P->len > 0){
        /* Stage 3.1 - Remove a group g from P */
        g = P->head->data;
        gLen = P->head->size;
        delete_first(P,0); /* 0 means deleting the node without freeing its data */

        /* Stage 3.2 - Divide g into g1 and g2 with Algorithm 2
         * And saves it into local variables */
        calc_two_division(B,my_division,g,gLen);
        g1 = my_division->g1;
        g2 = my_division->g2;
        g1_size = my_division->g1_size;
        g2_size = my_division->g2_size;

        /* Stage 3.3 - check if g1 or g2 is of size 0
         * Meaning that g is indivisible*/
        if(g1_size == 0 || g2_size == 0){
            insert_last(O, g, gLen);
        }
        else {
            /* Stage 3.4 - add groups to P or O*/
            if(g1_size == 1){
                insert_last(O, g1, g1_size);
            } else{
                insert_last(P, g1, g1_size);
            }
            if(g2_size == 1){
                insert_last(O, g2, g2_size);
            } else{
                insert_last(P, g2, g2_size);
            }
        }
    }

    /* Stage 4 - write the output division to File*/
    write_groups_to_file(O,locationOutput);

    /* free B*/
    free_modMat(B);
    free(B);

    /* free division struct
     * no need to use free_division(my_division), because the free happens on make_division and free_linkedList*/
    free(my_division);

    /* free linked list
     * no need to use free_linkedList(P), because P is empty here as described in Algorithm 3*/
    free_linkedList(O);
    free(P);
    free(O);

    argc = argc + 0;
    return 0;
}

/* initializing P = {{0,...,n-1}} in order to start the algorithm*/
void initialization_of_groups(linkedList *P, int n){
    int *trivial,i;

    /*Creating the trivial division of {1,2,...,n}*/
    trivial=(int*)malloc(n * sizeof(int));
    check_allocation(trivial,__LINE__,__FILE__);

    for(i=0; i<n; i++){
        trivial[i] = i;
    }

    /* P = {{0,...,n-1}}, O = {} */
    insert_first(P,trivial,n);
}

/* Write the groups into the output file and for each group
 * the function writes the group's size followed by the group's vertices in increasing order
 * (n_g)(n_g1)(g1)(n_g2)(g2)(n_g3)(g3)... */
void write_groups_to_file(linkedList *O, char *locationOutput){
    FILE *fOutput;
    int *group, size, len, check, i;
    node *tmp = O->head;

    fOutput = fopen(locationOutput,"w");
    check_open_file(fOutput,locationOutput,__LINE__,__FILE__);

    /*Writing number of groups in the division - (n_g) */
    len = O->len;
    check = fwrite(&len, sizeof(int), 1, fOutput);
    check_items_write(check,1,__LINE__,__FILE__);

    /*Writing for each group the group's size followed by the group's nodes in increasing order
     (n_g1)(g1)(n_g2)(g2)(n_g3)(g3)... */
    for(i=0; i<len; i++){
        group = tmp->data;
        size = tmp->size;

        /* Write n_g_i*/
        check = fwrite(&size, sizeof(int), 1, fOutput);
        check_items_write(check,1,__LINE__,__FILE__);

        /* Write g_i*/
        check = fwrite(group, sizeof(int), size, fOutput);
        check_items_write(check,size,__LINE__,__FILE__);

        tmp = tmp->next;
    }
    fclose(fOutput);
}
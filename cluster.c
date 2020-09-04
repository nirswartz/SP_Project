#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "errors.h"
#include "linkedList.h"
#include "divide.h"
#include "modMat.h"
/*deleteeeeeeeeeeeeeeeeeeeeee*/
#include "cluster.h"
#include "help.h"
/*deleteeeeeeeeeeeeeeeeeeeeee*/

/* function deceleration */
int main(int argc, char* argv[]);
void initialization_of_groups(linkedList *P, int n);
void write_groups_to_file(linkedList *O, char *locationOutput);
/* end of function deceleration */

/*change to mainnnnnnnnnnnnnnnnnnnnnnnnnn*/
/* The main algorithm which calculating the clusters according to Algorithm 3*/
int cluster(int argc, char* argv[]){
    char *locationInput, *locationOutput;
    int *g,*g1,*g2, gLen, size_g1, size_g2, **division;
    linkedList *O, *P;
    modMat *B;

    /*timing - deleteeeeeeeeeeeeeeeeeeeeeee*/
    clock_t t;
    double time_taken, vertexes;
    t = clock();

    locationInput = argv[1];
    locationOutput = argv[2];

    /*division[0]=g1, division[1]=g2, division[2]=size(g1), division[3]=size(g2)*/
    division = calloc(4, sizeof(int*));
    checkAllocation(division,__LINE__,__FILE__);
    division[2] = calloc(1, sizeof(int));
    checkAllocation(division[2],__LINE__,__FILE__);
    division[3]=calloc(1, sizeof(int));
    checkAllocation(division[3],__LINE__,__FILE__);

    /*Creating matrix B*/
    B = modMat_allocate(locationInput);

    /* Stage 1+2 - initialization Of P and O*/
    P = linkedList_allocate();
    O = linkedList_allocate();
    initialization_of_groups(P, B->n);

    /* Stage 3 - calculating the modularity groups*/
    while((P->len) > 0){
        /* Stage 3.1 - Remove a group g from P */
        g = P->head->data;
        gLen = P->head->size;

        /*printf("g is "); deleteeeeeeeeeeeeeeeeeeee
        printVectorInt(g,gLen);
        if(P->len==0){
            printf("P is empty\n");
        } else{
            printf("P is:\n");
            P->printList(P);
        }
        if(O->len==0){
            printf("O is empty\n");
        } else {
            printf("O is:\n");
            O->printList(P);
        }*/

        delete_first(P,0); /* 0 means deleting the node without freeing its data */

        /* Stage 3.2 - Divide g into g1,g2 with Algorithm 2*/
        calc_two_division(B,division,g,gLen);
        g1 = division[0];
        g2 = division[1];
        size_g1 = *division[2];
        size_g2 = *division[3];

        /* Stage 3.3 - check if g1 or g2 is of size 0*/
        if(size_g1 == 0 || size_g2 == 0){
            insert_last(O,g,gLen);
            free(g2); /*free garbage cell*/
        }
        else {
            /* Stage 3.4 - add groups to P or O*/
            if(size_g1 == 1){
                insert_last(O,g1,size_g1);
            } else{
                insert_last(P,g1,size_g1);
            }
            if(size_g2==1){
                insert_last(O,g2,size_g2);
            } else{
                insert_last(P,g2,size_g2);
            }
        }
    }

    /* Stage 4 - write the output division to File*/
    write_groups_to_file(O,locationOutput);

    /*delete*/
    vertexes=(double)B->n;
    argc=argc;
    /*delete*/

    /*free B*/
    free_modMat(B);
    free(B);

    /*free division*/
    free(division[2]);
    free(division[3]);
    free(division);

    /*free linkedList*/
    free_linkedList(P);
    free_linkedList(O);
    free(P);
    free(O);

    /*end timing - deleteeeeeeeeeeeeeee*/
    t = clock() - t;
    time_taken = (((double)t)/CLOCKS_PER_SEC); /* in seconds*/
    printf("\nRun for %f vertexes took %f seconds to execute \n",vertexes, time_taken);

    return 0;
}

/* initializing P = {{0,...,n-1}} in order to start the algorithm*/
void initialization_of_groups(linkedList *P, int n){
    int *trivial,i;
    /*Creating the trivial division of {1,2,...,n}*/
    trivial=(int*)malloc(n * sizeof(int));
    checkAllocation(trivial,__LINE__,__FILE__);
    for(i=0;i<n;i++){
        trivial[i] = i;
    }
    insert_first(P,trivial,n); /* P = {{0,...,n-1}}, O = {} */
}


/* Write to output file the groups
 * Writing for each group the group's size followed by the group's nodes in increasing order
 * (n_g)(n_g1)(g1)(n_g2)(g2)(n_g3)(g3)... */
void write_groups_to_file(linkedList *O, char *locationOutput){
    FILE *fOutput;
    int *group, size, len, check, i;
    node *tmp = O->head;

    fOutput = fopen(locationOutput,"w");
    checkOpenFile(fOutput,locationOutput,__LINE__,__FILE__);
    len = O->len;

    /*Writing number of groups in the division - (n_g) */
    check = fwrite(&len, sizeof(int), 1, fOutput);
    checkItemsWrite(check,1,__LINE__,__FILE__);

    /*Writing for each group the group's size followed by the group's nodes in increasing order
     (n_g)(n_g1)(g1)(n_g2)(g2)(n_g3)(g3)... */
    for(i=0;i<len;i++){
        group = tmp->data;
        size = tmp->size;
        check = fwrite(&size, sizeof(int), 1, fOutput); /*n_g_i*/
        checkItemsWrite(check,1,__LINE__,__FILE__);
        check = fwrite(group, sizeof(int), size, fOutput); /*g_i*/
        checkItemsWrite(check,size,__LINE__,__FILE__);
        tmp = tmp->next;
    }
    fclose(fOutput);
}

/*deleteeeeeeeeeeeeeeeeeee*/
int main(int argc, char* argv[]){
    test10();
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "help.h"
#include "errors.h"
#include "linkedList.h"
#include "divide.h"
#include "modMat.h"

void writeGroupsToFile(linkedList *O, char *locationOutput){
    FILE *fOutput;
    int *group, size, len, check, i;
    node *tmp=O->head;

    fOutput=fopen(locationOutput,"w");
    checkOpenFile(fOutput,locationOutput,__LINE__,__FILE__);
    len=O->len;

    /*Writing number of groups in the division*/
    check = fwrite(&len, sizeof(int), 1, fOutput);
    checkItemsWrite(check,1,__LINE__,__FILE__);

    /*Writing for each group the group's size followed by the group's nodes in increasing order
     (n_g)(n_g1)(g1)(n_g2)(g2)(n_g3)(g3)... */
    for(i=0;i<len;i++){
        group=tmp->data;
        size=tmp->size;
        check = fwrite(&size, sizeof(int), 1, fOutput); /*n_g_i*/
        checkItemsWrite(check,1,__LINE__,__FILE__);
        check = fwrite(group, sizeof(int), size, fOutput); /*g_i*/
        checkItemsWrite(check,size,__LINE__,__FILE__);
        tmp=tmp->next;
    }
    fclose(fOutput);
}

void initializationOfGroups(linkedList *O, linkedList *P, int n){
    int *trivial,i;

    /*Creating the trivial division of {1,2,...,n}*/
    trivial=(int*)malloc(n * sizeof(int));
    checkAllocation(trivial,__LINE__,__FILE__);
    for(i=1;i<=n;i++){
        trivial[i]=i;
    }

    P=linkedList_allocate();
    O=linkedList_allocate();
    P->insert_first(P,trivial,n);
}

int main(int argc, char* argv[]){
    /*declare all variables here*/
    char *locationInput, *locationOutput;
    FILE *fInput;
    int numOfNodes,check, *g,*g1,*g2, gLen, size_g1, size_g2, **division;
    linkedList *O, *P;

    locationInput=argv[1];
    locationOutput=argv[2];

    /*division[0]=g1, division[1]=g2, division[2]=size(g1), division[3]=size(g2)*/
    division=calloc(4, sizeof(int*));
    checkAllocation(division,__LINE__,__FILE__);

    /*Creating matrix B*/
    modMat *B=modMat_allocate(locationInput);

    /*Read Number of nodes (n) from the input file*/
    fInput=fopen(locationInput,"r");
    checkOpenFile(fInput,locationInput,__LINE__,__FILE__);
    check = fread(&numOfNodes, sizeof(int), 1, fInput);
    checkItemsRead(check,1,__LINE__,__FILE__);
    fclose(fInput);

    /* Stage 1+2 - initialization Of P and O*/
    initializationOfGroups(O, P, numOfNodes);

    /* Stage 3 - calculating the modularity groups*/
    while(P->len> 0 ){
        /* Stage 3.1 - Remove a group g from P*/
        g=P->head->data;
        gLen=P->head->size;
        P->delete_first(P,0);

        /* Stage 3.2 - Divide g into g1,g2 with Algorithm 2*/
        calcTwoDivision(B,division,g);
        g1=division[0];
        g2=division[1];
        size_g1=*division[2];
        size_g2=*division[3];

        /* Stage 3.3 - check if g1 or g2 is of size 0*/
        if(size_g1==0 || size_g2==0 ){
            O->insert_last(O,g,gLen);
        }

        /* Stage 3.4 - add groups to P or O*/
        if(size_g1==1){
            O->insert_last(O,g1,size_g1);
        } else{
            P->insert_last(P,g1,size_g1);
        }
        if(size_g2==1){
            O->insert_last(O,g2,size_g2);
        } else{
            P->insert_last(P,g2,size_g2);
        }
    }

    /* Stage 4 - write the output division to File*/
    writeGroupsToFile(O,locationOutput);

    /*free division*/
    free(g1);
    free(g2);
    free(division[2]);
    free(division[3]);
    free(division);

    /*free linkedList*/
    P->freeList(P);
    O->freeList(O);
    free(P);
    free(O);

    return 0;

}

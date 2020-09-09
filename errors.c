#include <stdio.h>
#include <stdlib.h>

/* functions deceleration */
void check_allocation(void* pointer, int rowNum, char *programName);
void check_items_read(int read, int shouldRead, int rowNum, char *programName);
void check_items_write(int write, int shouldWrite, int rowNum, char *programName);
void check_divide_by_zero(double num, int rowNum, char *programName);
void check_open_file(void *open, char *fileName, int rowNum, char *programName);
void check_empty_list(int len, int rowNum, char *programName);
void check_empty_graph(int size, int rowNum, char *programName);
void check_no_edges(int edgeNum, int rowNum, char *programName);
/*void check_getter_out_of_boundary(int i, int j, int n, int rowNum, char *programName);*/
/* end of functions deceleration */

void check_allocation(void* pointer, int rowNum, char *programName){
    if(pointer == NULL){
        printf("Allocation Failed in line: %d in program: %s\n",rowNum, programName);
        exit(10);
    }
}

void check_items_read(int read, int shouldRead, int rowNum, char *programName){
    if(read != shouldRead){
        printf("Read Failed! Should read %d items, but read %d items in line: %d in program: %s\n",shouldRead,read,rowNum, programName);
        exit(20);
    }
}

void check_items_write(int write, int shouldWrite, int rowNum, char *programName){
    if(write != shouldWrite){
        printf("Write Failed! Should write %d items, but write %d items in line: %d in program: %s\n",shouldWrite,write,rowNum, programName);
        exit(30);
    }
}

void check_divide_by_zero(double num, int rowNum, char *programName){
    if(num == 0.0){
        printf("Divide by zero in line: %d in program: %s\n",rowNum, programName);
        exit(40);
    }
}

void check_open_file(void *open, char *fileName, int rowNum, char *programName){
    if(open == NULL){
        printf("Couldn't open: %s in line: %d in program: %s\n",fileName,rowNum, programName);
        exit(50);
    }
}

/* checks if the list is empty before deleting a node  - Exit code 60*/
void check_empty_list(int len, int rowNum, char *programName){
    if(len == 0){
        printf("Couldn't delete a node / print list because the list is empty in line: %d in program: %s\n",rowNum, programName);
        exit(60);
    }
}

/* checks if the graph is empty (no nodes) - Exit code 70*/
void check_empty_graph(int size, int rowNum, char *programName){
    if(size == 0){
        printf("Couldn't compute groups because the graph is empty in line: %d in program: %s\n",rowNum, programName);
        exit(70);
    }
}

/* checks if the graph has no edges, so every division is legit - Exit code 80*/
void check_no_edges(int edgeNum, int rowNum, char *programName){
    if(edgeNum == 0){
        printf("The graph has no edges, so every division is legit with zero modularity in line: %d in program: %s\n",rowNum, programName);
        exit(80);
    }
}

/* checks if getter(i,j) is out of boundary for symmetric matrix n X n - Exit code 90*/
/*TODO: void check_getter_out_of_boundary(int i, int j, int n, int rowNum, char *programName){
    if(i > n || j > n || i < 0 || j < 0){
        printf("Out of boundary request for matrix %d X %d in line: %d in program: %s\n",n,rowNum, programName);
        exit(80);
    }
}*/

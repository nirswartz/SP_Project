#include <stdio.h>
#include <stdlib.h>

void checkAllocation(void* pointer){
    if(pointer==NULL){
    printf("Allocation Failed!\n");
    exit(10);
    }
}

void checkItemsRead(int read, int shouldRead){
    if(read!=shouldRead){
        printf("Read Failed! Should read %d items, but read %d items\n",read,shouldRead);
        exit(20);
    }
}

void checkItemsWrite(int write, int shouldWrite){
    if(write!=shouldWrite){
        printf("Write Failed! Should write %d items, but write %d items\n",write,shouldWrite);
        exit(30);
    }
}


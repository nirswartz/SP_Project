#include <stdio.h>
#include <stdlib.h>

void checkAllocation(void* pointer, int rowNum){
    if(pointer==NULL){
        printf("Allocation Failed! (Row %d)\n",rowNum);
        exit(10);
    }
}

void checkItemsRead(int read, int shouldRead, int rowNum){
    if(read!=shouldRead){
        printf("Read Failed! Should read %d items, but read %d items! (Row %d)\n",shouldRead,read,rowNum);
        exit(20);
    }
}

void checkItemsWrite(int write, int shouldWrite, int rowNum){
    if(write!=shouldWrite){
        printf("Write Failed! Should write %d items, but write %d items! (Row %d)\n",shouldWrite,write,rowNum);
        exit(30);
    }
}

void checkDivideByZero(int num, int rowNum){
    if(num==0){
        printf("Divide by zero! (Row %d)",rowNum);
        exit(40);
    }
}

void checkOpenFile(void* open, char *fileName, int rowNum, char* programFile){
    if(open==NULL){
        printf("Couldn't open: %s in line: %d in program: %s! \n",fileName,rowNum, programFile);
        exit(50);
    }
}



#include <stdio.h>
#include <stdlib.h>


void checkAllocation(void* pointer, int rowNum, char *programName){
    if(pointer==NULL){
        printf("Allocation Failed in line: %d in program: %s\n",rowNum, programName);
        exit(10);
    }
}

void checkItemsRead(int read, int shouldRead, int rowNum, char *programName){
    if(read!=shouldRead){
        printf("Read Failed! Should read %d items, but read %d items in line: %d in program: %s\n",shouldRead,read,rowNum, programName);
        exit(20);
    }
}

void checkItemsWrite(int write, int shouldWrite, int rowNum, char *programName){
    if(write!=shouldWrite){
        printf("Write Failed! Should write %d items, but write %d items in line: %d in program: %s\n",shouldWrite,write,rowNum, programName);
        exit(30);
    }
}

void checkDivideByZero(int num, int rowNum, char *programName){
    if(num==0){
        printf("Divide by zero in line: %d in program: %s\n",rowNum, programName);
        exit(40);
    }
}

void checkOpenFile(void *open, char *fileName, int rowNum, char *programName){
    if(open==NULL){
        printf("Couldn't open: %s in line: %d in program: %s\n",fileName,rowNum, programName);
        exit(50);
    }
}

/* checks if the list is empty before deleting a node  - Exit code 60*/
void checkEmptyList(int len, int rowNum, char *programName){
    if(len==0){
        printf("Couldn't delete a node / print list because the list is empty in line: %d in program: %s\n",rowNum, programName);
        exit(60);
    }
}

/*int checkAllocation(void* pointer, int rowNum, char *programName){
    if(pointer==NULL){
        printf("Allocation Failed in line: %d in program: %s\n",rowNum, programName);
        return 10;
    }
    return 0;
}

int checkItemsRead(int read, int shouldRead, int rowNum, char *programName){
    if(read!=shouldRead){
        printf("Read Failed! Should read %d items, but read %d items in line: %d in program: %s\n",shouldRead,read,rowNum, programName);
        return 20;
    }
    return 0;
}

int checkItemsWrite(int write, int shouldWrite, int rowNum, char *programName){
    if(write!=shouldWrite){
        printf("Write Failed! Should write %d items, but write %d items in line: %d in program: %s\n",shouldWrite,write,rowNum, programName);
        return 30;
    }
    return 0;
}

int checkDivideByZero(int num, int rowNum, char *programName){
    if(num==0){
        printf("Divide by zero in line: %d in program: %s\n",rowNum, programName);
        return 40;
    }
    return 0;
}

int checkOpenFile(void *open, char *fileName, int rowNum, char *programName){
    if(open==NULL){
        printf("Couldn't open: %s in line: %d in program: %s\n",fileName,rowNum, programName);
        return 50;
    }
    return 0;
}
*/


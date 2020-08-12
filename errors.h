#ifndef SP_PROJECT_ERRORS_H
#define SP_PROJECT_ERRORS_H
/* ########################################### add __FILE__, change comments, change on code#####################################*/
// checks if the last allocation was successful - Exit code 10
void checkAllocation(void* pointer, int rowNum, char *programName);

// checks if the last fread was successful - Exit code 20
void checkItemsRead(int read, int shouldRead, int rowNum, char *programName);

// checks if the last fwrite was successful - Exit code 30
void checkItemsWrite(int write, int shouldWrite, int rowNum, char *programName);

// checks if dividing by zero - Exit code 40
void checkDivideByZero(int num, int rowNum, char *programName);

// checks if the last fopen was successful  - Exit code 50
void checkOpenFile(void *open, char *fileName, int rowNum, char *programName);

#endif //SP_PROJECT_ERRORS_H

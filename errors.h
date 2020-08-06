#ifndef SP_PROJECT_ERRORS_H
#define SP_PROJECT_ERRORS_H

// checks if the last allocation was successful - Exit code 10
void checkAllocation(void* pointer);

// checks if the last fread was successful - Exit code 20
void checkItemsRead(int read, int shouldRead);

// checks if the last fwrite was successful - Exit code 30
void checkItemsWrite(int write, int shouldWrite);

#endif //SP_PROJECT_ERRORS_H

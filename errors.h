#ifndef SP_PROJECT_ERRORS_H
#define SP_PROJECT_ERRORS_H

/* This module allows efficient error handling, each function returns the problematic line and the name of the program */

/*checks if the last allocation was successful - Exit code 10*/
void check_allocation(void* pointer, int rowNum, char *programName);

/* checks if the last fread was successful - Exit code 20*/
void check_items_read(int read, int shouldRead, int rowNum, char *programName);

/* checks if the last fwrite was successful - Exit code 30*/
void check_items_write(int write, int shouldWrite, int rowNum, char *programName);

/* checks if dividing by zero - Exit code 40*/
void check_divide_by_zero(double num, int rowNum, char *programName);

/* checks if the last fopen was successful  - Exit code 50*/
void check_open_file(void *open, char *fileName, int rowNum, char *programName);

/* checks if the list is empty before deleting a node  - Exit code 60*/
void check_empty_list(int len, int rowNum, char *programName);

/* checks if the graph is empty (no nodes) - Exit code 70*/
void check_empty_graph(int size, int rowNum, char *programName);

/* checks if the graph has no edges, so every division is legit - Exit code 80*/
void check_no_edges(int edgeNum, int rowNum, char *programName);

/* checks if number of iterations is beyond expected which is like infinite loop - Exit code 90*/
void check_infinite_loop(int totalIters, int upperBound, int rowNum, char *programName);

#endif
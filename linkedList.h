#ifndef SP_PROJECT_LINKEDLIST_H
#define SP_PROJECT_LINKEDLIST_H

/* This class implements linked list with two pointers to head and tails of the list
 * Each node hold two field: data and size
 * data is a point to int vector and size is len(data) */

typedef struct _node{
    int *data;
    int size;
    struct  _node *next;
} node;

typedef struct _linkedList{
    node *head;
    node *tail;
    int len;
} linkedList;

/*Allocates a new linked List*/
linkedList* linkedList_allocate();

/*Insert new node to the list at the first place*/
void insert_first(linkedList *list, int *data, int size);

/*Insert new node to the list at the last place*/
void insert_last(linkedList *list, int *data, int size);

/* Delete the first node from the list.
 * If contentFlag == 1, so the data will be deleted as well
 * Otherwise the data won't be freed */
void  delete_first(linkedList *list, int contentFlag);

/*Free all nodes include the data*/
void free_linkedList(linkedList *list);

#endif

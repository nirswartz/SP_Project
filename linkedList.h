#ifndef SP_PROJECT_LINKEDLIST_H
#define SP_PROJECT_LINKEDLIST_H

typedef struct _node{
    int *data;
    int size;
    struct  _node *next;
}node;

typedef struct _linkedList{
    node *head;
    node *tail;
    int len;
}linkedList;

/*Insert new node to the list at the first place*/
void insert_first(linkedList *list, int *data, int size);

/*Insert new node to the list at the last place*/
void insert_last(linkedList *list, int *data, int size);

/*Delete the first node from the list. If contentFlag==1, so the data will be deleted as well*/
void  delete_first(linkedList *list, int contentFlag);

/*Free all nodes include the data*/
void free_linkedList(linkedList *list);

/*Allocates a new linkedList*/
linkedList* linkedList_allocate();

/*Print the list - deleteeeeee*/
void printList(linkedList *list);

#endif

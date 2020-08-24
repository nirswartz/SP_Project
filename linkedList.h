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

    /*Insert new node to the list at the first place*/
    void (*insert_first)(struct _linkedList *list, int *data, int size);

    /*Insert new node to the list at the last place*/
    void (*insert_last)(struct _linkedList *list, int *data, int size);

    /*Delete the first node from the list. If contentFlag==1, so the data will be deleted as well*/
    void  (*delete_first)(struct _linkedList *list, int contentFlag);

    /*Delete the last node from the list. If contentFlag==1, so the data will be deleted as well*/
    /*O(n)!*/
    void  (*delete_last)(struct _linkedList *list, int contentFlag);

    /*void (*deleteNode)(struct _linkedList *list, node nodeDelete);*/

    /*Print the list*/
    void (*printList)(struct _linkedList *list);

    /*Free all nodes include the data*/
    void (*freeList)(struct _linkedList *list);

}linkedList;

linkedList *linkedList_allocate();

#endif

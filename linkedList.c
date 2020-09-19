#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"
#include "errors.h"

/*function declaration*/
linkedList* linkedList_allocate();
void insert_first(linkedList *list, int *data, int size);
void insert_last(linkedList *list, int *data, int size);
void  delete_first(linkedList *list, int contentFlag);
void delete_list_by_head(node *head);
void free_linkedList(linkedList *list);
/*end of functions declaration*/

linkedList* linkedList_allocate(){
    linkedList *list = (linkedList*) malloc(sizeof(linkedList));
    check_allocation(list, __LINE__,__FILE__);
    list->len = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

/*Insert new node to the list at the first place*/
void insert_first(linkedList *list, int* data, int size){
    node *tmp = (node*)malloc(sizeof(node));
    check_allocation(tmp,__LINE__,__FILE__);
    tmp->data = data;
    tmp->size = size;
    tmp->next = NULL;
    list->len++;
    /*The list is empty*/
    if(list->head == NULL){
        list->head = tmp;
        list->tail = tmp;
        return;
    }
    tmp->next = list->head;
    list->head = tmp;
}

/*Insert new node to the list at the last place*/
void insert_last(linkedList *list, int* data, int size){
    node *tmp = (node*)malloc(sizeof(node));
    check_allocation(tmp,__LINE__,__FILE__);
    tmp->data = data;
    tmp->size = size;
    tmp->next = NULL;
    list->len++;
    /*The list is empty*/
    if(list->tail == NULL){
        list->head = tmp;
        list->tail = tmp;
        return;
    }
    list->tail->next = tmp;
    list->tail = tmp;
}

/* Delete the first node from the list.
 * If contentFlag == 1, so the data will be deleted as well
 * Otherwise the data won't be freed */
void delete_first(linkedList *list, int contentFlag){
    node *tmp;
    check_empty_list(list->len,__LINE__,__FILE__);
    tmp = list->head;
    if(list->len == 1){
        list->head = NULL;
        list->tail = NULL;
    }
    else{
        list->head = list->head->next;
    }
    if(contentFlag == 1){
        free(tmp->data);
    }
    free(tmp);
    list->len--;
}

/*use recursive way to delete all nodes from the end to the beginning*/
void delete_list_by_head(node *head){
    if(head != NULL){
        delete_list_by_head(head->next);
        free(head->data);
        free(head);
    }
}

/*Free all nodes include the data*/
void free_linkedList(linkedList *list){
    delete_list_by_head(list->head);
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
}

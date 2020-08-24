#include "linkedList.h"
#include "errors.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>

/*Insert new node to the list at the first place*/
void insert_first(struct _linkedList *list, int* data, int size){
    node *tmp=(node*)malloc(sizeof(node));
    checkAllocation(tmp,__LINE__,__FILE__);
    tmp->data=data;
    tmp->size=size;
    tmp->next=NULL;
    list->len++;
    /*The list is empty*/
    if(list->head==NULL){
        list->head=tmp;
        list->tail=tmp;
        return;
    }
    tmp->next=list->head;
    list->head=tmp;
}

/*Insert new node to the list at the last place*/
void insert_last(struct _linkedList *list, int* data, int size){
    node *tmp=(node*)malloc(sizeof(node));
    checkAllocation(tmp,__LINE__,__FILE__);
    tmp->data=data;
    tmp->size=size;
    tmp->next=NULL;
    list->len++;
    /*The list is empty*/
    if(list->tail==NULL){
        list->head=tmp;
        list->tail=tmp;
        return;
    }
    list->tail->next=tmp;
    list->tail=tmp;
}

/*Delete the first node from the list. If contentFlag==1, so the data will be deleted as well*/
void delete_first(struct _linkedList *list, int contentFlag){
    checkEmptyList(list->len,__LINE__,__FILE__);
    node *tmp=list->head;
    if(list->len==1){
        list->head=NULL;
        list->tail=NULL;
    } else{
        list->head=list->head->next;
    }
    if(contentFlag==1){
        free(tmp->data);
    }
    free(tmp);
    list->len--;
}

/*Delete the last node from the list. If contentFlag==1, so the data will be deleted as well*/
/*O(n)!*/
void delete_last(struct _linkedList *list, int contentFlag){
    checkEmptyList(list->len,__LINE__,__FILE__);
    node *tmp=list->head;
    if(list->len==1){
        list->head=NULL;
        list->tail=NULL;
    } else{
        while(tmp->next != list->tail){
            tmp=tmp->next;
        }
        list->tail=tmp;
        tmp=tmp->next;
        list->tail->next=NULL;
    }
    if(contentFlag==1){
        free(tmp->data);
    }
    free(tmp);
    list->len--;
}

/*
void deleteNode(linkedList *list, node nodeDelete){

}*/

/*Print the list*/
void printList(struct _linkedList *list){
    checkEmptyList(list->len,__LINE__,__FILE__);
    int len=list->len,i,j,*val;
    node *tmp=list->head;
    printf("The list has %d nodes: ",len);
    for(i=0;i<len-1;i++){
        printf("{");
        val=tmp->data;
        for(j=0; j<tmp->size-1;j++){
            printf("%d , ",val[j]);
        }
        if(tmp->size>0) {
            printf("%d} -> ", val[j]);
        } else{
            printf("} -> ");
        }
        tmp=tmp->next;
    }
    printf("{");
    val=tmp->data;
    for(j=0; j<tmp->size-1;j++){
        printf("%d , ",val[j]);
    }
    if(tmp->size>0){
        printf("%d} -> NULL\n",val[j]);
    } else{
        printf("} -> NULL\n");
    }

}

/*use recursive way to delete all nodes from the end to the beginning*/
void deleteListByHead(node *head){
    if(head!=NULL){
        deleteListByHead(head->next);
        free(head->data);
        free(head);
    }
}

/*Free all nodes include the data*/
void freeList(struct _linkedList *list){
    deleteListByHead(list->head);
    list->head=NULL;
    list->tail=NULL;
    list->len=0;
}

linkedList* linkedList_allocate(){
    linkedList *list = malloc(sizeof(linkedList));
    checkAllocation(list, __LINE__,__FILE__);
    list->len = 0;
    list->head=NULL;
    list->tail=NULL;
    list->insert_first=&insert_first;
    list->insert_last=&insert_last;
    list->delete_first=&delete_first;
    list->delete_last = &delete_last;
    /*list->deleteNode=&deleteNode;*/
    list->printList=&printList;
    list->freeList=&freeList;
    return list;
}
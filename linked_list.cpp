#include "linked_list.h"

struct Node *CreateList() {
    return NULL;
}

int insert(struct Node **list, int name) {
    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
    temp->data = name;
    temp->next = *list ;
    *list = temp;
    return 0;
}

void append(struct Node *list, int data) {
    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
    temp->data = data;
    temp->next = NULL;
    while (list->next != NULL) {
        list = list->next;
    }
    list->next = temp;
}

int del(struct Node **list, int index) {//改成二重指针
    struct Node **change = list;
    struct Node *temp1;
    for (int i = 0; i < index; i++) {
        temp1 = *change;
        if ((*change)->next == NULL) {
            return 1;
        }
        change = &(temp1->next);
    }
    free(*change);
    *change =(*change)->next ;
    return 0;
}

int find(struct Node *list, int data, int start) {
    for (int i=0; list != NULL;list = list->next,i++) {
        if ( i>=start && list->data == data) {
            return i;
        }
    }
    return -1;
}
//
// Created by 13578 on 2025/11/14.
//

#ifndef C_GOBANG_LINKED_LIST_H
#define C_GOBANG_LINKED_LIST_H
#include <stdio.h>
#include <stdlib.h>
struct Node{
    int data;
    struct Node *next;
};
struct Node *CreateList();
int insert(struct Node **list, int name);
void append(struct Node *list, int data);
int del(struct Node **list, int index);
//int find(struct Node *list, int data, int start);

#endif //C_GOBANG_LINKED_LIST_H
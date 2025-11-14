//
// Created by 13578 on 2025/10/20.
//

#ifndef C_GOBANG_GOBANG_H
#define C_GOBANG_GOBANG_H
#include <stdio.h>
#include <stdlib.h>

#define MAXBRANCHES 10
#define MAXSHAPES 20

typedef enum {neighbor11x=0,neighbor110,jump1010,jump101x,three111x,three1110,threejump11010,threejump1101x,threejump10110,threejump1011x,four1111x,four11110,five}Shape;
//typedef enum {neighbor11=1,three111x=2,three1110=10,four1111x=15,four11110=100,five=200}ShapeValue;
typedef enum {machine=1,human=-1}Player;
typedef struct {
    char X;
    char Y;
}Point;
typedef struct {
    char dirX;
    char dirY;
}Direction;

Shape BoardState[15][15][4];

struct Node{
    int data;
    struct Node *next;
};
struct Node *CreateList();
int insert(struct Node **list, int name);
void append(struct Node *list, int data);
int del(struct Node **list, int index);
//int find(struct Node *list, int data, int start);

Point *generate_choices();
void next_state(Point point);
void de_state(Shape** state,Point point);

void search(Point point ,int length,Player player);//每层搜索直接编辑BoardState,出口前回退状态

#endif //C_GOBANG_GOBANG_H
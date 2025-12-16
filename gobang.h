//
// Created by 13578 on 2025/10/20.
//

#ifndef C_GOBANG_GOBANG_H
#define C_GOBANG_GOBANG_H
#include "linked_list.h"

#define MAXBRANCHES 10
#define MAXDEPTH 6
#define BOARD_SIZE 15
#define COMPUTER 1
#define EMPTY 0
#define HUMAN -1

#define SCORE_SINGLE 1
#define SCORE_NEIGHBOR_BLOCKED 2
#define SCORE_NEIGHBOR_FREE 10
#define SCORE_THREE_BLOCKED 100
#define SCORE_THREE_FREE 1000
#define SCORE_FOUR_BLOCKED 1800
#define SCORE_FOUR_FREE 10000
#define SCORE_WIN 100000

#define SCORE_LOSE -1000000


typedef enum
{
    null = 0, single, neighbor0110, neighbor011x, neighborx110, neighbor_dead, jump01010, jump0101x, jumpx1010,
    jump_dead, three01110, three0111x, threex1110, three_dead, threejump011010, threejump01101x, threejumpx11010,
    threejump1101dead, threejump010110, threejump01011x, threejumpx10110, threejump1011dead, four011110, four01111x,
    fourx11110, four_dead, fourjump11101, fourjump10111, fourjump11011, five
} Ownerless_Shape;
//typedef enum {neighbor11=1,three111x=2,three1110=10,four1111x=15,four11110=100,five=200}ShapeValue;

typedef struct
{
    bool owner            :1;
    //Ownerless_Shape shape :7;//删
    bool isblocked_begin  :1;
    bool isblocked_end    :1;
    char length0          :7;
    char length1          :6;
}Shape;
typedef struct {
    char X;
    char Y;
}Point;
typedef struct {
    char dirX;
    char dirY;
}Direction;

const short value_free[]={1,10,1000,10000};
const short value_blocked[]={1,2,200,2000};

typedef enum
{
    end_of_change = 0,
    shape_create,
    shape_destroy,
    score_change,
    heat_change,
}ChangeMode;
typedef struct
{
    ChangeMode mode;
    union
    {
        struct
        {
            Shape shape;
            Point position;
            char direction;
        }shapeData;

        struct
        {
            Point position;
            int score;
        }heat;
        int score;
    }data;
}ChangeLog;

#define STACK_SIZE 200
ChangeLog change_stack[STACK_SIZE];
ChangeLog *ptr_change_stack;
void stack_init();
ChangeLog stack_pop();
void shape_push(ChangeMode mode,Point position,Shape shape,char direction);
void score_push(int score);
void heat_push(Point position, int score);
void start_push();

char  Board     [15][15]   ;
Shape BoardState[15][15][4];
int HeatMap   [15][15]   ;
int   Score;
Point FinalChoice;

bool is_within_board(char X, char Y);

Point *generate_choices();
bool next_state(Point point,bool isMachine);
void de_state();//用栈
bool change(Point point,char dir,char player);

int search(Point point, int depth, bool isMachine, int alpha_beta);//每层搜索直接编辑BoardState,出口前回退状态
int get_value();
#endif //C_GOBANG_GOBANG_H
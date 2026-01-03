#ifndef C_GOBANG_GOBANG_H
#define C_GOBANG_GOBANG_H

#include <cstring>
#include <cstdlib>

#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))

#define MAXBRANCHES 20
#define MAXDEPTH 7
#define BOARD_SIZE 15
#define COMPUTER 1
#define EMPTY 0
#define HUMAN (-1)

#define SCORE_WIN 100000

typedef struct
{
    bool owner            :1;
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

typedef struct Node{
    struct
    {
        int heat;
        Point point;
    }data;
    Node *next;
}Node;

const short value_free[]={1,10,100,1000,10000};
const short value_blocked[]={1,2,20,200,10000};

typedef enum
{
    end_of_change = 0,
    shape_create,
    shape_del,
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

#define STACK_SIZE 2000

#ifdef __cplusplus
extern "C" {
#endif

    // 原有的C函数声明
    bool is_within_board(char X, char Y);
    bool checkWin(int x, int y, char player);
    Point *generate_choices();
    bool next_state(Point point,bool isMachine);
    void de_state(Point point);
    bool change(Point point,char dir,char player);
    int search(int depth, bool isMachine, int alpha_beta);
    int get_value();
    void board_init();
    void stack_init();
    ChangeLog stack_pop();
    void shape_push(ChangeMode mode, Point position, Shape shape, char direction);
    void score_push(int score);
    void heat_push(Point position, int score);
    void start_push();

    // 全局变量声明
    extern char Board[15][15];
    extern Shape BoardState[15][15][4];
    extern int HeatMap[15][15];
    extern int Score;
    extern Point FinalChoice;
    extern ChangeLog change_stack[STACK_SIZE];
    extern ChangeLog *ptr_change_stack;
    extern char firstPlayer;

#ifdef __cplusplus
}
#endif
#endif //C_GOBANG_GOBANG_H

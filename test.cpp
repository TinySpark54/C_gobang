

#include <cstdio>
#include <cstdlib>
typedef struct {
    char X;
    char Y;
}Point;
typedef struct Node{
    struct
    {
        int heat;
        Point point;
    }data;

    Node *next;
}Node;

#define MAXBRANCHES 7
#define BOARD_SIZE 4
int HeatMap[4][4] ={
    {1,0,4,2},
    {2,1,0,3},
    {0,0,3,1},
    {0,1,0,0}
};


Point *generate_choices()
{
    Point *choices = (Point *)malloc(sizeof(Point)*MAXBRANCHES);
    Node *list = nullptr;
    Node **ptr;
    int heat;
    Node *new_node;

    int step = 0;
    int min = 0;
    for (char i = 0; i < BOARD_SIZE; i++)
    {
        for (char j = 0; j < BOARD_SIZE; j++)
        {
            if (HeatMap[i][j] != 0)
            {
                heat = HeatMap[i][j];

                if (heat > min)
                {
                    step = 0;
                    ptr = &list;
                    for (; *ptr != nullptr &&(*ptr)->data.heat>heat; ptr=&(*ptr)->next,step++){}
                    new_node = (Node *)malloc(sizeof(Node));
                    *new_node = {{heat,{j,i}},*ptr};
                    *ptr = new_node;
                    for (; (*ptr)->next != nullptr && step < MAXBRANCHES; ptr=&(*ptr)->next,step++){}
                    if (step == MAXBRANCHES)
                    {
                        min = (*ptr)->data.heat;
                    }

                }
            }
        }
    }
    ptr = &list;
    step = 0;
    for (; *ptr != nullptr && step < MAXBRANCHES; *ptr=(*ptr)->next,step++)
    {
        choices[step] = (*ptr)->data.point;
    }
    ptr = &list;
    Node *ptr2;
    while (*ptr != nullptr)
    {
        ptr2 = *ptr;
        *ptr=(*ptr)->next;
        free(ptr2);
    }
    return choices;
}
int main()
{
    Point *choice = generate_choices();
    for (char i = 0; i < MAXBRANCHES; i++)
    {
        printf("{%d,%d}\n",choice[i].X,choice[i].Y);
    }

    free(choice);
}

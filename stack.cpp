#include "gobang.h"
void stack_init()
{
    ptr_change_stack = &change_stack[0];
}

ChangeLog stack_pop()
{
    return *--ptr_change_stack;
}
void shape_push(ChangeMode mode,Point position,Shape shape,char direction)
{
    ChangeLog log;
    log.mode=mode;
    log.data.shapeData.position=position;
    log.data.shapeData.direction=direction;
    log.data.shapeData.shape=shape;
    *ptr_change_stack++ = log;
}
void score_push(int score)
{
    ChangeLog log;
    log.mode=score_change;
    log.data.score=score;
    *ptr_change_stack++ = log;
}
void heat_push(Point position, int score)
{
    ChangeLog log;
    log.mode=heat_change;
    log.data.heat.position=position;
    log.data.heat.score=score;
    *ptr_change_stack++ = log;
}
void start_push()
{
    ptr_change_stack++->mode=end_of_change;
}

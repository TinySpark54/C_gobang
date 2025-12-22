#ifdef __cplusplus
extern "C" {
#endif


#include "gobang.h"

ChangeLog change_stack[STACK_SIZE];
ChangeLog *ptr_change_stack;

void stack_init()
{
    ptr_change_stack = &change_stack[0];
}

ChangeLog stack_pop()
{
    return *--ptr_change_stack;
}

void shape_push(ChangeMode mode, Point position, Shape shape, char direction)
{
    ChangeLog log;
    log.mode = mode;
    log.data.shapeData.shape = shape;
    log.data.shapeData.position = position;
    log.data.shapeData.direction = direction;

    *ptr_change_stack++ = log;

    char add = (mode == shape_create) ? 1 : -1;

    if (mode == shape_create)
    {
        BoardState[position.Y][position.X][direction] = shape;
    }
    else
    {
        BoardState[position.Y][position.X][direction].length0 = 0;
    }

    char total_pieces = shape.length0 + shape.length1;
    char total_length = total_pieces + (shape.length1 != 0 ? 1 : 0);
    char player = shape.owner ? 1 : -1;
    char deltaX = direction ? (2 - direction) : 1;
    char deltaY = direction ? 1 : 0;
    char front_X = position.X - deltaX;
    char front_Y = position.Y - deltaY;
    char back_X = position.X + total_length * deltaX;
    char back_Y = position.Y + total_length * deltaY;

    if (shape.length1 != 0)
    {
        char broke_X = position.X + shape.length0 * deltaX;
        char broke_Y = position.Y + shape.length0 * deltaY;

        if (shape.isblocked_begin)
        {
            if (shape.isblocked_end)
            {
                if (total_pieces >= 4)
                {
                    score_push(value_blocked[3] * player * add);
                    heat_push({broke_X, broke_Y}, SCORE_WIN * add);
                }
            }
            else
            {
                heat_push({back_X, back_Y}, ((shape.length0 > 2) ? value_free[shape.length0] :
                          value_blocked[(total_pieces < 4) ? total_pieces : 3]) * add);
                heat_push({broke_X, broke_Y}, value_blocked[total_pieces] * add);
                score_push(((shape.length0 > 3) ? value_free[shape.length0 - 1] :
                           value_blocked[(total_pieces < 4) ? total_pieces : 3]) * add * player);
            }
        }
        else
        {
            if (shape.isblocked_end)
            {
                heat_push({front_X, front_Y}, ((shape.length0 > 2) ? value_free[shape.length0] :
                          value_blocked[(total_pieces < 4) ? total_pieces : 3]) * add);
                heat_push({broke_X, broke_Y}, value_blocked[total_pieces] * add);
                score_push(((shape.length0 > 2) ? value_free[shape.length0 - 1] :
                           value_blocked[(total_pieces < 4) ? total_pieces : 3]) * add * player);
            }
            else
            {
                heat_push({back_X, back_Y}, ((total_pieces < 3) ? value_free[total_pieces] :
                          value_blocked[3]) * add);
                heat_push({front_X, front_Y}, ((total_pieces < 3) ? value_free[total_pieces] :
                          value_blocked[3]) * add);
                heat_push({broke_X, broke_Y}, value_blocked[total_pieces] * add);
                score_push(value_blocked[(total_pieces < 4) ? total_pieces : 3] * player * add);
            }
        }
    }
    else if (!shape.isblocked_end)
    {
        if (shape.isblocked_begin)
        {
            heat_push({back_X, back_Y}, value_blocked[total_pieces] * add);
            score_push(value_blocked[total_pieces] * add * player);
        }
        else
        {
            heat_push({back_X, back_Y}, value_free[total_pieces] * add);
            heat_push({front_X, front_Y}, value_free[total_pieces] * add);
            score_push(value_free[total_pieces] * add * player);
        }
    }
    else if (!shape.isblocked_begin)
    {
        heat_push({front_X, front_Y}, value_blocked[total_pieces] * add);
        score_push(value_blocked[total_pieces] * add * player);
    }
}

void score_push(int score)
{
    ChangeLog log;
    log.mode = score_change;
    log.data.score = score;
    *ptr_change_stack++ = log;
    Score += score;
}

void heat_push(Point position, int score)
{
    ChangeLog log;
    log.mode = heat_change;
    log.data.heat.position = position;
    log.data.heat.score = score;
    *ptr_change_stack++ = log;
    HeatMap[position.Y][position.X] += score;
}

void start_push()
{
    ptr_change_stack->mode = end_of_change;
    ptr_change_stack++;
}
#ifdef __cplusplus
}
#endif
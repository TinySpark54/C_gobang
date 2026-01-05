#include "gobang.h"
#include <cstdlib>
#include <cstring>
#include <stdio.h>

char Board[15][15];
Shape BoardState[15][15][4];
int HeatMap[15][15];
int Score;
Point FinalChoice;
char firstPlayer;

// 胜利检查函数
bool checkWin(int x, int y, char player)
{
    // 检查八个方向（四个直线方向）
    int directions[4][2] = {
        {1, 0},  // 水平
        {0, 1},  // 垂直
        {1, 1},  // 主对角线
        {1, -1}  // 副对角线
    };

    for (int d = 0; d < 4; d++)
    {
        int dx = directions[d][0];
        int dy = directions[d][1];
        int count = 1; // 当前位置的棋子

        // 正向检查
        for (int i = 1; i <= 4; i++)
        {
            int newX = x + i * dx;
            int newY = y + i * dy;

            if (!is_within_board(newX, newY) || Board[newY][newX] != player)
                break;

            count++;
        }

        // 反向检查
        for (int i = 1; i <= 4; i++)
        {
            int newX = x - i * dx;
            int newY = y - i * dy;

            if (!is_within_board(newX, newY) || Board[newY][newX] != player)
                break;

            count++;
        }

        // 如果连续5个或以上
        if (count >= 5)
            return true;
    }

    return false;
}

void board_init()
{
    Score = 0;
    for(int i = 0; i < BOARD_SIZE; i++)
    {
        for(int j = 0; j < BOARD_SIZE; j++)
        {
            Board[i][j] = 0;
            HeatMap[i][j] = 0;
            for(int dir = 0; dir < 4; dir++)
            {
                BoardState[i][j][dir].owner = 0;
                BoardState[i][j][dir].isblocked_begin = 0;
                BoardState[i][j][dir].isblocked_end = 0;
                BoardState[i][j][dir].length0 = 0;
                BoardState[i][j][dir].length1 = 0;
            }
        }
    }
}

int search(int depth, bool isMachine, int alpha_beta)//搜索主函数
{
    if (depth > 0)
    {
        int value = isMachine ? -1000000 : 1000000;
        int temp;
        Point *choices = generate_choices();

        for (int i = 0; i < MAXBRANCHES; i++)
        {
            if (choices[i].X == -1) break;
            if (Board[choices[i].Y][choices[i].X] != EMPTY)
            {
                printf("严重错误：重复落子！");
                break;
            }
            // 尝试走这一步

            // 检查是否获胜；若未获胜，变更局面
            if (next_state(choices[i], isMachine))
            {

                if (depth == MAXDEPTH)
                {
                    FinalChoice = choices[i];
                }
                // 获胜，结束递归
                free(choices);
                return isMachine ? SCORE_WIN : -SCORE_WIN;
            }

            temp = search(depth - 1, !isMachine, value);

            if (isMachine)
            {
                if (temp > value)
                {
                    value = temp;
                    if (depth == MAXDEPTH)
                    {
                        FinalChoice = choices[i];
                    }
                    else if (temp >= alpha_beta)
                    {
                        de_state(choices[i]);
                        free(choices);
                        return alpha_beta;
                    }
                }
            }
            else
            {
                if (temp < value)
                {
                    value = temp;
                    if (temp <= alpha_beta)
                    {
                        de_state(choices[i]);
                        free(choices);
                        return alpha_beta;
                    }
                }
            }

            de_state(choices[i]);
        }
        free(choices);
        return value;
    }
    return Score;
}

//局势变化
bool next_state(Point point, bool isMachine)
{
    char player = isMachine ? COMPUTER : HUMAN;

    // 先检查是否立即获胜
    if (checkWin(point.X, point.Y, player))
    {
        return true;
    }

    Board[point.Y][point.X] = player;
    start_push();
    heat_push(point, -HeatMap[point.Y][point.X]);

    // 更新四个方向的状态
    for (int i = 0; i < 4; i++)
    {
        change(point, i, player);
    }

    return false;
}

//单方向的局势变化处理
bool change(Point point, char dir, char player)
{
    Shape shape_temp;
    char deltaX = dir ? (2 - dir) : 1;
    char deltaY = dir ? 1 : 0;
    char Xrightnear = point.X + deltaX;
    char Yrightnear = point.Y + deltaY;
    bool is_alive = is_within_board(Xrightnear, Yrightnear);
    bool ismachine = (player == COMPUTER);

    // 左边界或左敌
    if (!is_within_board(point.X - deltaX, point.Y - deltaY) ||
        Board[point.Y - deltaY][point.X - deltaX] == -player)
    {
        if (Board[point.Y - deltaY][point.X - deltaX] == -player)
        {
            // 左敌情况
            for (int i = 1; ; i++)
            {
                char Xi = point.X - i * deltaX;
                char Yi = point.Y - i * deltaY;
                if (!is_within_board(Xi, Yi)) break;

                if (BoardState[Yi][Xi][dir].length0 != 0)
                {
                    shape_temp = BoardState[Yi][Xi][dir];
                    shape_push(shape_del, {Xi, Yi}, shape_temp, dir);

                    int total_length = (shape_temp.length0 + shape_temp.length1) +
                                       (shape_temp.length1 != 0 ? 1 : 0);

                    if (total_length == i)
                    {
                        shape_temp.isblocked_end = true;
                        shape_push(shape_create, {Xi, Yi}, shape_temp, dir);

                        if (!is_alive && is_within_board(Xrightnear, Yrightnear))
                        {
                            shape_temp = BoardState[Yrightnear][Xrightnear][dir];
                            shape_push(shape_del, {Xrightnear, Yrightnear}, shape_temp, dir);
                            shape_temp.isblocked_begin = true;
                            shape_push(shape_create, {Xrightnear, Yrightnear}, shape_temp, dir);
                        }
                    }
                    else
                    {
                        // 切断敌方棋形
                        Shape shape_left;
                        Shape shape_right;

                        if (!shape_temp.isblocked_begin)
                        {
                            shape_left = shape_temp;
                            shape_left.length1 = 0;
                            shape_left.isblocked_end = true;
                            shape_push(shape_create, {Xi, Yi}, shape_left, dir);
                        }

                        shape_right.owner = (player == COMPUTER) ? 0 : 1;
                        shape_right.length0 = shape_temp.length1;
                        shape_right.isblocked_begin = true;
                        shape_right.length1 = 0;
                        shape_right.isblocked_end = false;
                        shape_push(shape_create, {point.X, point.Y}, shape_right, dir);

                        is_alive = false;
                    }
                    break;
                }
            }

            if (is_alive)
            {
                if (is_within_board(Xrightnear, Yrightnear))
                {
                    if (Board[Yrightnear][Xrightnear] == player)
                    {
                        shape_temp = BoardState[Yrightnear][Xrightnear][dir];
                        shape_push(shape_del, {Xrightnear, Yrightnear}, shape_temp, dir);
                        shape_temp.length0++;
                        shape_temp.isblocked_begin = true;
                        shape_push(shape_create, {Xrightnear, Yrightnear}, shape_temp, dir);
                    }
                    else if (Board[Yrightnear][Xrightnear] == -player)
                    {
                        shape_temp = BoardState[Yrightnear][Xrightnear][dir];
                        shape_push(shape_del, {Xrightnear, Yrightnear}, shape_temp, dir);
                        shape_temp.isblocked_begin = true;
                        shape_push(shape_create, {Xrightnear, Yrightnear}, shape_temp, dir);
                        shape_temp.owner = ismachine;
                        shape_temp.isblocked_begin = true;
                        shape_temp.isblocked_end = true;
                        shape_temp.length0 = 1;
                        shape_temp.length1 = 0;
                    }
                    else
                    {
                        shape_temp.owner = ismachine;
                        shape_temp.isblocked_begin = true;
                        shape_temp.isblocked_end = false;
                        shape_temp.length0 = 1;
                        shape_temp.length1 = 0;
                    }
                }
                else
                {
                    shape_temp.owner = ismachine;
                    shape_temp.isblocked_begin = true;
                    shape_temp.isblocked_end = true;
                    shape_temp.length0 = 1;
                    shape_temp.length1 = 0;
                }
            }
            else
            {
                shape_temp.owner = ismachine;
                shape_temp.isblocked_begin = true;
                shape_temp.isblocked_end = true;
                shape_temp.length0 = 1;
                shape_temp.length1 = 0;
            }

            shape_push(shape_create, point, shape_temp, dir);
        }
    }
    else if (Board[point.Y - deltaY][point.X - deltaX] == player)
    {
        // 左己情况
        for (int i = 1; ; i++)
        {
            char Xi = point.X - i * deltaX;
            char Yi = point.Y - i * deltaY;
            if (!is_within_board(Xi, Yi)) break;

            if (BoardState[Yi][Xi][dir].length0 != 0)
            {
                shape_temp = BoardState[Yi][Xi][dir];
                shape_push(shape_del, {Xi, Yi}, shape_temp, dir);

                if (shape_temp.length1 == 0)
                {
                    shape_temp.length0++;

                    if (is_within_board(Xrightnear, Yrightnear))
                    {
                        if (Board[Yrightnear][Xrightnear] == player)
                        {
                            Shape shape_right = BoardState[Yrightnear][Xrightnear][dir];
                            shape_push(shape_del, {Xrightnear, Yrightnear}, shape_right, dir);
                            shape_temp.length0 += shape_right.length0;
                            shape_temp.length1 = shape_right.length1;
                            shape_temp.isblocked_end = shape_right.isblocked_end;
                            shape_push(shape_create, {Xi, Yi}, shape_temp, dir);
                        }
                        else if (Board[Yrightnear][Xrightnear] == -player)
                        {
                            shape_temp.isblocked_end = true;
                            shape_push(shape_create, {Xi, Yi}, shape_temp, dir);

                            shape_temp = BoardState[Yrightnear][Xrightnear][dir];
                            shape_push(shape_del, {Xrightnear, Yrightnear}, shape_temp, dir);
                            shape_temp.isblocked_begin = true;
                            shape_push(shape_create, {Xrightnear, Yrightnear}, shape_temp, dir);
                        }
                        else
                        {
                            char Xrightspace = Xrightnear + deltaX;
                            char Yrightspace = Yrightnear + deltaY;
                            if (is_within_board(Xrightspace, Yrightspace) &&
                                Board[Yrightspace][Xrightspace] == player)
                            {
                                Shape shape_right = BoardState[Yrightspace][Xrightspace][dir];
                                if (shape_right.length1 == 0)
                                {
                                    shape_temp.length1 = shape_right.length0;
                                    shape_temp.isblocked_end = shape_right.isblocked_end;
                                    shape_push(shape_del, {Xrightspace, Yrightspace}, shape_right, dir);
                                    shape_push(shape_create, {Xi, Yi}, shape_temp, dir);
                                }
                            }
                            else
                            {
                                shape_push(shape_create, {Xi, Yi}, shape_temp, dir);
                            }
                        }
                    }
                    else
                    {
                        shape_temp.isblocked_end = true;
                        shape_push(shape_create, {Xi, Yi}, shape_temp, dir);
                    }
                }
                else
                {
                    shape_temp.length1++;

                    if (is_within_board(Xrightnear, Yrightnear))
                    {
                        if (Board[Yrightnear][Xrightnear] == -player)
                        {
                            shape_temp.isblocked_end = true;
                            shape_push(shape_create, {Xi, Yi}, shape_temp, dir);

                            shape_temp = BoardState[Yrightnear][Xrightnear][dir];
                            shape_push(shape_del, {Xrightnear, Yrightnear}, shape_temp, dir);
                            shape_temp.isblocked_begin = true;
                            shape_push(shape_create, {Xrightnear, Yrightnear}, shape_temp, dir);
                        }
                        else if (Board[Yrightnear][Xrightnear] == player)
                        {
                            Shape shape_right = BoardState[Yrightnear][Xrightnear][dir];
                            shape_push(shape_del, {Xrightnear, Yrightnear}, shape_right, dir);
                            shape_temp.length1 += shape_right.length0;
                            shape_push(shape_create, {Xi, Yi}, shape_temp, dir);
                        }
                    }
                    shape_push(shape_create, {Xi, Yi}, shape_temp, dir);
                }
                break;
            }
        }
    }
    else if (is_within_board(point.X - deltaX * 2, point.Y - deltaY * 2) &&
             Board[point.Y - deltaY * 2][point.X - deltaX * 2] == player)
    {
        // 左跳一己情况
        for (int i = 1; ; i++)
        {
            char Xi = point.X - i * deltaX;
            char Yi = point.Y - i * deltaY;
            if (!is_within_board(Xi, Yi)) break;

            if (BoardState[Yi][Xi][dir].length0 != 0)
            {
                if (BoardState[Yi][Xi][dir].length1 == 0)
                {
                    shape_temp = BoardState[Yi][Xi][dir];
                    shape_push(shape_del, {Xi, Yi}, shape_temp, dir);
                    shape_temp.length1++;

                    if (is_within_board(Xrightnear, Yrightnear) &&
                        Board[Yrightnear][Xrightnear] == player)
                    {
                        Shape shape_right = BoardState[Yrightnear][Xrightnear][dir];
                        shape_push(shape_del, {Xrightnear, Yrightnear}, shape_right, dir);
                        shape_temp.length1 += shape_right.length0;
                        shape_temp.isblocked_end = shape_right.isblocked_end;
                    }
                    else if (is_within_board(Xrightnear, Yrightnear) &&
                             Board[Yrightnear][Xrightnear] == -player)
                    {
                        shape_temp.isblocked_end = true;
                        Shape shape_right = BoardState[Yrightnear][Xrightnear][dir];
                        shape_push(shape_del, {Xrightnear, Yrightnear}, shape_right, dir);
                        shape_right.isblocked_begin = true;
                        shape_push(shape_create, {Xrightnear, Yrightnear}, shape_right, dir);
                    }
                    shape_push(shape_create, {Xi, Yi}, shape_temp, dir);
                }
                break;
            }
        }
    }
    else
    {
        // 孤子情况
        if (is_within_board(Xrightnear, Yrightnear))
        {
            char Xrightspace = Xrightnear + deltaX;
            char Yrightspace = Yrightnear + deltaY;

            if (Board[Yrightnear][Xrightnear] == player)
            {
                shape_temp = BoardState[Yrightnear][Xrightnear][dir];
                shape_push(shape_del, {Xrightnear, Yrightnear}, shape_temp, dir);
                shape_temp.length0++;
                shape_push(shape_create, point, shape_temp, dir);
            }
            else if (Board[Yrightnear][Xrightnear] == -player)
            {
                shape_temp.owner = ismachine;
                shape_temp.isblocked_begin = false;
                shape_temp.isblocked_end = true;
                shape_temp.length0 = 1;
                shape_temp.length1 = 0;

                Shape shape_right = BoardState[Yrightnear][Xrightnear][dir];
                shape_push(shape_del, {Xrightnear, Yrightnear}, shape_right, dir);
                shape_right.isblocked_begin = true;
                shape_push(shape_create, {Xrightnear, Yrightnear}, shape_right, dir);
                shape_push(shape_create, point, shape_temp, dir);
            }
            else if (is_within_board(Xrightspace, Yrightspace) &&
                     Board[Yrightspace][Xrightspace] == player &&
                     BoardState[Yrightspace][Xrightspace][dir].length1 == 0)
            {
                shape_temp = BoardState[Yrightspace][Xrightspace][dir];
                shape_push(shape_del, {Xrightspace, Yrightspace}, shape_temp, dir);
                shape_temp.length1 = shape_temp.length0;
                shape_temp.length0 = 1;
                shape_push(shape_create, point, shape_temp, dir);
            }
            else
            {
                shape_temp.owner = ismachine;
                shape_temp.isblocked_begin = false;
                shape_temp.isblocked_end = false;
                shape_temp.length0 = 1;
                shape_temp.length1 = 0;
                shape_push(shape_create, point, shape_temp, dir);
            }
        }
        else
        {
            shape_temp.owner = ismachine;
            shape_temp.isblocked_begin = false;
            shape_temp.isblocked_end = true;
            shape_temp.length0 = 1;
            shape_temp.length1 = 0;
            shape_push(shape_create, point, shape_temp, dir);
        }
    }

    return false; // 不在这里检查胜利
}

void de_state(Point point)//回退状态
{
    Board[point.Y][point.X] = EMPTY;
    bool loop = true;
    ChangeLog change_log;

    while (loop)
    {
        change_log = stack_pop();
        switch (change_log.mode)
        {
        case end_of_change:
            loop = false;
            break;
        case shape_create:
            BoardState[change_log.data.shapeData.position.Y]
                      [change_log.data.shapeData.position.X]
                      [change_log.data.shapeData.direction].length0 = 0;
            break;
        case shape_del:
            BoardState[change_log.data.shapeData.position.Y]
                      [change_log.data.shapeData.position.X]
                      [change_log.data.shapeData.direction] =
                change_log.data.shapeData.shape;
            break;
        case score_change:
            Score -= change_log.data.score;
            break;
        case heat_change:
            HeatMap[change_log.data.heat.position.Y]
                   [change_log.data.heat.position.X] -=
                change_log.data.heat.score;
            break;
        }
    }
}

Point *generate_choices()//启发选点
{
    Point *choices = (Point *)malloc(sizeof(Point) * MAXBRANCHES);
    for (int i = 0; i < MAXBRANCHES; i++)
    {
        choices[i].X = -1;
        choices[i].Y = -1;
    }

    Node *list = nullptr;
    int count = 0;

    for (char i = 0; i < BOARD_SIZE; i++)
    {
        for (char j = 0; j < BOARD_SIZE; j++)
        {
            if (Board[i][j] == EMPTY && HeatMap[i][j] != 0)
            {
                int heat = HeatMap[i][j];
                Node *new_node = (Node *)malloc(sizeof(Node));
                new_node->data.heat = heat;
                new_node->data.point.X = j;
                new_node->data.point.Y = i;
                new_node->next = nullptr;

                if (list == nullptr || heat > list->data.heat)
                {
                    new_node->next = list;
                    list = new_node;
                }
                else
                {
                    Node *current = list;
                    while (current->next != nullptr && current->next->data.heat > heat)
                    {
                        current = current->next;
                    }
                    new_node->next = current->next;
                    current->next = new_node;
                }
                count++;
            }
        }
    }

    // 如果没有热力图启发，选择中心附近的空位
    if (count == 0)
    {
        choices[0].X=7;
        choices[0].Y=7;
    }
    else
    {
        Node *current = list;
        for (int i = 0; i < MAXBRANCHES && current != nullptr; i++)
        {
            choices[i] = current->data.point;
            current = current->next;
        }
    }

    // 释放链表内存
    while (list != nullptr)
    {
        Node *temp = list;
        list = list->next;
        free(temp);
    }

    return choices;
}


bool is_within_board(char X, char Y)
{
    return X >= 0 && X < BOARD_SIZE && Y >= 0 && Y < BOARD_SIZE;
}

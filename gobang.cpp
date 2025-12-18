#include "gobang.h"

#include <cstring>

void init_board()
{
    Score = 0;
    for(int i=0;i<BOARD_SIZE;i++)
    {
        for(int j=0;j<BOARD_SIZE;j++)
        {
            Board[i][j]=0;
            HeatMap[i][j]=0;
            for(int dir=0;dir<4;dir++)
            {
                memset(&BoardState[i][j][dir],0,2);

            }
        }
    }
}

int search(Point point, int depth, bool isMachine, int alpha_beta) {

    if (depth>0)
    {
        int value=100000;
        if (isMachine)
        {
            value=-value;
        }
        int temp;
        Point *choices = generate_choices();
        for (int i = 0; i < MAXBRANCHES; i++)
        {
            if (next_state(choices[i],isMachine))
            {
                de_state();
                return isMachine?SCORE_WIN:-SCORE_LOSE;
            }
            temp = search(choices[i],depth-1,!isMachine,value);

            if (isMachine && (temp > value))
            {
                value = temp;
                if (depth==MAXDEPTH)
                {
                    FinalChoice = choices[i];
                }
                else if (temp >= alpha_beta)
                {
                    de_state();
                    return alpha_beta;
                }
            }
            else if (!isMachine && (temp < value))
            {
                value = temp;
                if (temp <= alpha_beta)
                {
                    de_state();
                    return alpha_beta;
                }
            }

            de_state();
        }
        return value;
    }
    return Score;
}//释放内存

bool next_state(Point point, bool isMachine)//不能用void,半路成五立即返回
{
    char player = isMachine ? 1 : -1;
    Board[point.Y][point.X] = player;
    start_push();
    heat_push(point,-HeatMap[point.Y][point.X]);
    HeatMap[point.Y][point.X]=0;
    for (int i = 0; i < 4; i++) {
        if (change(point, i, player))
        {
            return true;
        }

    }
    return false;

}
//使用读取前方格state并处理，已知问题：难以表示长连；直接在过程中修改分数，不再最终估值
bool change(Point point, char dir, char player)
{
    char deltaX;
    char deltaY;
    int total_length;
    int total_pieces;
    int heat_temp;
    int score_temp;
    bool is_alive = is_within_board(point.X + deltaX,point.Y + deltaY);
    Shape shape_temp;

    switch (dir)
    {
    case 0: deltaX = 1;        deltaY = 0;        break;
    case 1: deltaX = 1;        deltaY = 1;        break;
    case 2: deltaX = 0;        deltaY = 1;        break;
    case 3: deltaX = -1;       deltaY = 1;        break;
    default: return;
    }
    if (!is_within_board(point.X - deltaX, point.Y - deltaY) || Board[point.Y - deltaY][point.X - deltaX] == -player)//左堵
    {
        if (Board[point.Y - deltaY][point.X - deltaX] == -player)//左敌
        {
            for (int i = 1;; i++)
            {
                char Xi = point.X - i * deltaX;
                char Yi = point.Y - i * deltaY;
                if (BoardState[Yi][Xi][dir].length0 != 0)
                {
                    shape_temp = BoardState[Yi][Xi][dir];
                    total_pieces = shape_temp.length0 + shape_temp.length1;
                    total_length = total_pieces + (shape_temp.length1 == 0 ? 0 : 1);
                    BoardState[Yi][Xi][dir].isblocked_end = true;
                    //左敌左查
                    if (total_length <= i)
                    {
                        shape_push(shape_destroy, {(char)Yi, (char)Xi},
                                   shape_temp, dir);

                        char Ybroke = point.Y - (shape_temp.length1 + 1) * deltaY;
                        char Xbroke = point.X - (shape_temp.length1 + 1) * deltaX;
                        if (!shape_temp.isblocked_begin)
                        {
                            heat_temp = value_blocked[total_pieces];
                            score_temp = (total_length>4)?0:player*value_blocked[total_pieces-1];
                            Score += score_temp;
                            score_push(score_temp);
                            if (shape_temp.length1 > 0)
                            {
                                heat_push({Ybroke,Xbroke}, heat_temp);
                                HeatMap[Ybroke][Xbroke] += heat_temp;

                            }
                            if (shape_temp.length1 == 0 || total_length<4)
                            {
                                heat_push({
                                          (char)(point.Y - (total_length + 1) * deltaY),
                                          (char)(point.X - (total_length + 1) * deltaX)
                                      }, heat_temp);
                                HeatMap[point.Y - (total_length + 1) * deltaY][point.X - (total_length + 1) * deltaX] += heat_temp;

                            }
                        }
                        else if (total_length < 5)
                        {
                            score_temp = player * value_blocked[total_pieces - 1];
                            if (shape_temp.length1 != 0)
                            {
                                heat_push({
                                              Ybroke,
                                              (char)Xbroke
                                          }, -value_blocked[shape_temp.length0 + shape_temp.length1]);
                                HeatMap[point.Y - (shape_temp.length1+1) * deltaY][Xbroke] = 0;
                            }
                        }

                    }
                    else//切断了敌方棋形
                    {
                        shape_push(shape_destroy,{
                                              (char)Yi,
                                              (char)Xi
                                          },shape_temp, dir);
                        //处理前半段
                        if (!shape_temp.isblocked_begin)
                        {
                            if (!is_within_board(point.X-(i+2)*deltaX, point.Y-(i+2)*deltaY) ||Board[point.Y-(i+2)*deltaY][point.X-(i+2)*deltaX] != player)
                            {
                                break_front_inline:

                                BoardState[Yi][Xi][dir].length1 = 0;
                                heat_temp = value_blocked[shape_temp.length0-1]-(shape_temp.isblocked_end?value_blocked:value_free)[total_pieces-1];
                                HeatMap[point.Y - (i+1) * deltaY][point.X - (i+1) * deltaX] += heat_temp;
                                heat_push({
                                              (char)(point.Y - (i+1) * deltaY),
                                              (char)(point.X - (i+1) * deltaX)
                                          },heat_temp);
                            }
                            else
                            {
                                score_temp = player * value_blocked[total_pieces-1];
                                score_push(score_temp);
                                Score += score_temp;
                                for (int j=2;;j++)
                                {
                                    char Yj = point.Y - (i+j) * deltaY;
                                    char Xj = point.X - (i+j) * deltaX;
                                    if (BoardState[BoardState[Yj][Xj][dir].length0 != 0 ])
                                    {
                                        if (BoardState[Yj][Xj][dir].length1 ==0)
                                        {
                                            shape_push(shape_destroy,{Yj,Xj},BoardState[Yj][Xj][dir],dir);
                                            BoardState[Yj][Xj][dir].length1 = shape_temp.length0;
                                            BoardState[Yj][Xj][dir].isblocked_end = true;
                                            BoardState[Yi][Xi][dir].length0 = 0;

                                            if (BoardState[Yj][Xj][dir].isblocked_begin)
                                            {
                                                if (BoardState[Yj][Xj][dir].length0+BoardState[Yj][Xj][dir].length1 >3)
                                                {
                                                    heat_temp = SCORE_WIN;
                                                }else
                                                {
                                                    heat_temp = 0;
                                                    score_temp = player * value_blocked[BoardState[Yj][Xj][dir].length0+BoardState[Yj][Xj][dir].length1-1];
                                                    score_push(score_temp);
                                                    Score += score_temp;
                                                }

                                                heat_push({(char)(Xi-1),(char)(Yi-1)},heat_temp);
                                                HeatMap[Yi-1][Xi-1]=heat_temp;
                                            }
                                            else
                                            {
                                                heat_temp = value_blocked[BoardState[Yj][Xj][dir].length0+BoardState[Yj][Xj][dir].length1-1];
                                                heat_push({(char)(Xi-1),(char)(Yi-1)},heat_temp);
                                                HeatMap[Yi-1][Xi-1]=heat_temp;
                                                heat_push({(char)(Xj-1),(char)(Yj-1)},heat_temp);
                                                HeatMap[Yj-1][Xj-1]=heat_temp;
                                            }
                                        }
                                        else goto break_front_inline;
                                        break;
                                    }
                                }
                            }
                        }
                        //处理后半段
                        shape_temp.length0 = shape_temp.length1;
                        shape_temp.length1 = 0;
                        is_alive = false;
                    }
                    break;
                }
            }
        }
        //左敌右查
        if (is_alive)
        {
            if (Board[point.Y + deltaY][point.X + deltaX] == player) //连
            {
                shape_temp = BoardState[point.Y + deltaY][point.X + deltaX][dir];
                shape_temp.length0++;
                if (shape_temp.length0 == 5)
                {
                    return true;
                }
                shape_temp.isblocked_begin == true;
                BoardState[point.Y][point.X][dir] = shape_temp;
                shape_push(shape_create, point, shape_temp, dir);
                total_pieces = shape_temp.length0 + shape_temp.length1;
                total_length = total_pieces + (shape_temp.length1 == 0 ? 0 : 1);
                heat_temp = value_blocked[total_pieces] - value_free[total_pieces - 1];

                if (!shape_temp.isblocked_end)
                {
                    //分值表的第0位为一子
                    HeatMap[point.Y + deltaY * (total_length + 1)][point.X + deltaX * (total_length + 1)] +=
                        heat_temp;
                    heat_push({
                                  (char)(point.Y + deltaY * (total_length + 1)),
                                  (char)(point.X + deltaX * (total_length + 1))
                              }, heat_temp);
                    score_temp = player * (value_blocked[total_pieces - 1] - value_free[total_pieces - 2]);
                }
                else if (total_length < 5)
                {
                    heat_temp = -value_blocked[total_pieces - 1];
                    score_temp = -player * value_blocked[total_pieces - 2];
                }
                if (shape_temp.length1 != 0)
                {
                    //中间断开一律算冲四

                    HeatMap[point.Y + deltaY * (shape_temp.length0 + 1)][point.X + deltaX * (shape_temp.length0
                        + 1)] += heat_temp;
                    heat_push({
                                  (char)(point.Y + deltaY * (shape_temp.length0 + 1)),
                                  (char)(point.X + deltaX * (shape_temp.length0 + 1))
                              }, heat_temp);
                    if (total_length >= 5)
                    {
                        score_temp = player * (value_blocked[4 - 1] - value_blocked[3 - 1]);
                    }
                }
                shape_push(shape_destroy, {(char)(point.Y + deltaY), (char)(point.X + deltaX)}, shape_temp,
                           dir);
                BoardState[point.Y + deltaY][point.X + deltaX][dir].length0 = 0;

                score_push(score_temp);
            }
            else if (Board[point.Y + deltaY][point.X + deltaX] == EMPTY)
            {
                if (is_within_board(point.X + deltaX * 2,point.Y + deltaY * 2) && Board[point.Y + deltaY * 2][point.X + deltaX * 2] == player && BoardState[point.Y + deltaY * 2][point.X + deltaX * 2][dir].length1 ==0)//跳
                {

                }
                else
                {

                }
            }
            else //空/死
            {
            }
        }




        else if (Board[point.Y-deltaY][point.X-deltaX] == player || Board[point.Y-deltaY*2][point.X-deltaX*2] == player)
        {
            for (int i = 1;; i++)
            {
                if (BoardState[point.Y-i*deltaY][point.X-i*deltaX][dir].length0 != 0)
                {
                    switch (i)
                    {
                        case 1:

                            break;
                        case 2:break;
                        case 3:break;
                        case 4:break;
                        case 5:break;
                        default:break;
                    }
                    //重构和热力图\分数修改
                    return;
                }
            }
        }
    }
    else if (Board[point.Y-deltaY][point.X-deltaX] == player)//左己
    {

    }
    else if (is_within_board(point.X-deltaX*2,point.Y-deltaY*2) ||Board[point.Y-deltaY*2][point.X-deltaX*2] == player)//左跳一己
    {

    }
    else//孤子
    {

    }

}
void de_state() {
    bool loop = true;
    ChangeLog change_log;
    while (loop) {
        change_log = stack_pop();
        switch (change_log.mode) {
            case end_of_change:
                loop = false;
                break;
            case shape_create:
                BoardState[change_log.data.shapeData.position.Y][change_log.data.shapeData.position.X][change_log.data.shapeData.direction].length0 =0;
                break;
            case shape_destroy:
                BoardState[change_log.data.shapeData.position.Y][change_log.data.shapeData.position.X][change_log.data.shapeData.direction] = change_log.data.shapeData.shape;
                break;
            case score_change:
                Score -= change_log.data.score;
                break;
            case heat_change:
                HeatMap[change_log.data.heat.position.Y][change_log.data.heat.position.X] -= change_log.data.heat.score;
                break;
        }
    }
}

//是否减少jump棋形以降低case数？不用了，没有棋形枚举了已经
//改变棋形表示方法，两位分别表示两头死活，**内部直接用对应二进制**(改为断点前后两段长度，连跳直接拆分)，再找规律直接加加减减？
//先不搞禁手了

bool is_within_board(char X, char Y)
{
    return X>=0 && X<BOARD_SIZE && Y>=0 && Y<BOARD_SIZE;
}
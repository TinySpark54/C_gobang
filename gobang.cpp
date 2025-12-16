#include "gobang.h"


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
            next_state(choices[i],isMachine);
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

}
//使用读取前方格state并处理，已知问题：难以表示长连；直接在过程中修改分数，不再最终估值
bool change(Point point,char dir,char player)
{
    char deltaX;
    char deltaY;
    int total_length;
    int total_pieces;
    int heat_temp;
    int score_temp;
    Shape shape_temp;

    switch (dir)
    {
        case 0:deltaX = 1;deltaY = 0;break;
        case 1:deltaX = 1;deltaY = 1;break;
        case 2:deltaX = 0;deltaY = 1;break;
        case 3:deltaX =-1;deltaY = 1;break;
        default:return false;
    }
    if (is_within_board(point.X-deltaX, point.Y-deltaY))
    {
        if (Board[point.Y-deltaY][point.X-deltaX] == -player) {
            //逆向削减对方
            for (int i = 1;; i++)
            {
                if (BoardState[point.Y-i*deltaY][point.X-i*deltaX][dir].length0 != 0)
                {
                    shape_temp = BoardState[point.Y-i*deltaY][point.X-i*deltaX][dir];
                    shape_push(shape_destroy, {(char) (point.Y - i * deltaY), (char) (point.X - i * deltaX)},
                               shape_temp, dir);
                    shape_temp.isblocked_end == true;
                    if (!shape_temp.isblocked_begin) {
                        heat_temp = value_blocked[shape_temp.length0 + shape_temp.length1 -1]
                                     - value_free[shape_temp.length0 + shape_temp.length1 -1];
                        heat_push({
                                          (char) (point.Y - (i + 1) * deltaY),
                                          (char) (point.X - (i + 1) * deltaX)
                                      },heat_temp);
                        HeatMap[point.Y - (i + 1) * deltaY][point.X - (i + 1) * deltaX] += heat_temp;

                    }
                    else if (shape_temp.length1 ==0 || shape_temp.length0 +shape_temp.length1 <4) {
                        score_temp = -player*value_blocked[shape_temp.length0 +shape_temp.length1 -1];
                        if (shape_temp.length1 !=0) {
                            //heat_temp = -value_blocked[shape_temp.length0 +shape_temp.length1];
                            heat_push({
                                          (char) (point.Y - (i - shape_temp.length0) * deltaY),
                                          (char) (point.X - (i - shape_temp.length0) * deltaX)
                                      }, -value_blocked[shape_temp.length0 + shape_temp.length1]);
                            HeatMap[point.Y - (i - shape_temp.length0) * deltaY][point.X - (i - shape_temp.length0) * deltaX] = 0;
                        }
                    }
                    //重构和热力图\分数修改
                    break;
                }
            }

            if (Board[point.Y+deltaY][point.X+deltaX] == player) {
                shape_temp = BoardState[point.Y+deltaY][point.X+deltaX][dir];
                shape_temp.length0 ++;
                if (shape_temp.length0==5) {
                    return true;
                }
                shape_temp.isblocked_begin == true;
                BoardState[point.Y][point.X][dir] = shape_temp;
                shape_push(shape_create,point,shape_temp,dir);
                total_pieces = shape_temp.length0 +shape_temp.length1;
                total_length = total_pieces + (shape_temp.length1 ==0?0:1);
                heat_temp = value_blocked[total_pieces]-value_free[total_pieces-1];

                if (!shape_temp.isblocked_end ) {
                    //分值表的第0位为一子
                    HeatMap[point.Y+deltaY*(total_length+1)][point.X+deltaX*(total_length+1)] +=heat_temp;
                    heat_push({
                                  (char)(point.Y + deltaY * (total_length + 1)),
                                  (char)(point.X + deltaX * (total_length + 1))
                              }, heat_temp);
                    score_temp = player*value_blocked[total_pieces-1]- value_free[total_pieces-2];
                }
                else if (total_length < 5)
                {
                    heat_temp = -value_blocked[total_pieces-1];
                    score_temp =-player*value_blocked[total_pieces-2];
                }
                if (shape_temp.length1 != 0) {//中间断开一律算冲四

                    HeatMap[point.Y+deltaY*(shape_temp.length0+1)][point.X+deltaX*(shape_temp.length0+1)] +=heat_temp;
                    heat_push({
                                  (char)(point.Y + deltaY * (shape_temp.length0 + 1)),
                                  (char)(point.X + deltaX * (shape_temp.length0 + 1))
                              }, heat_temp);
                    if (total_length >= 5) {
                        score_temp = player*(value_blocked[4-1]- value_blocked[3-1]);
                    }
                }
                shape_push(shape_destroy, {(char)(point.Y + deltaY), (char)(point.X + deltaX)}, shape_temp, dir);
                BoardState[point.Y+deltaY][point.X+deltaX][dir].shape =null;

                score_push(score_temp);

            }
            else if (Board[point.Y+deltaY][point.X+deltaX] == EMPTY && Board[point.Y+deltaY*2][point.X+deltaX*2] == player) {

            }
            //正向搜索

        }
        else if (Board[point.Y-deltaY][point.X-deltaX] == player || Board[point.Y-deltaY*2][point.X-deltaX*2] == player)
        {
            for (int i = 1;; i++)
            {
                if (BoardState[point.Y-i*deltaY][point.X-i*deltaX][dir].shape)
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
                HeatMap[change_log.data.heat.position.Y][change_log.data.heat.position.X] = change_log.data.heat.score;
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
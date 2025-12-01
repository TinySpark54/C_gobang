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
                    de_state(choices[i]);
                    return alpha_beta;
                }
            }
            else if (!isMachine && (temp < value))
            {
                value = temp;
                if (temp <= alpha_beta)
                {
                    de_state(choices[i]);
                    return alpha_beta;
                }
            }

            de_state(choices[i]);
        }
        return value;
    }
    return get_value();
}//释放内存

void next_state(Point point, bool isMachine)//不能用void,半路成五立即返回
{
    char player = isMachine ? 1 : -1;
    Board[point.Y][point.X] = player;
    change(point,0,player);
    change(point,1,player);
    change(point,2,player);
    change(point,3,player);

}
//使用读取前方格state并处理，已知问题：难以表示长连；直接在过程中修改分数，不再最终估值
void change(Point point,char dir,char player)
{
    char deltaX;
    char deltaY;
    int total_length;
    int total_pieces;
    switch (dir)
    {
        case 0:deltaX = 1;deltaY = 0;break;
        case 1:deltaX = 1;deltaY = 1;break;
        case 2:deltaX = 0;deltaY = 1;break;
        case 3:deltaX =-1;deltaY = 1;break;
        default:return;
    }
    if (point.X >=deltaX && point.Y >=deltaY &&point.X - deltaX <= BOARD_SIZE)
    {
        if (Board[point.Y-deltaY][point.X-deltaX] == -player) {
            //逆向削减对方
            for (int i = 1;; i++)
            {
                if (BoardState[point.Y-i*deltaY][point.X-i*deltaX][dir].owner==-player)
                {
                    BoardState[point.Y-i*deltaY][point.X-i*deltaX][dir].shape =(Ownerless_Shape)(BoardState[point.Y-i*deltaY][point.X-i*deltaX][dir].shape+1);
                    //重构和热力图\分数修改
                    break;
                }
            }

            if (Board[point.Y+deltaY][point.X+deltaX] == player) {
                BoardState[point.Y][point.X][dir] = BoardState[point.Y+deltaY][point.X+deltaX][dir];
                BoardState[point.Y][point.X][dir].length0 ++;
                BoardState[point.Y][point.X][dir].isblocked_begin == true;
                total_pieces = BoardState[point.Y][point.X][dir].length0 +BoardState[point.Y][point.X][dir].length1;
                total_length = total_pieces + (BoardState[point.Y][point.X][dir].length1 ==0?0:1);
                if (!BoardState[point.Y][point.X][dir].isblocked_end) {
                    HeatMap[point.Y+deltaY*(total_length+1)][point.X+deltaX*(total_length+1)] +=(value_blocked[total_pieces]-value_free[total_pieces-1]);
                }
                if (BoardState[point.Y][point.X][dir].length1 != 0) {

                }
                BoardState[point.Y+deltaY][point.X+deltaX][dir].shape =null;
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

//是否减少jump棋形以降低case数？
//改变棋形表示方法，两位分别表示两头死活，**内部直接用对应二进制**(改为断点前后两段长度，连跳直接拆分)，再找规律直接加加减减？
//先不搞禁手了
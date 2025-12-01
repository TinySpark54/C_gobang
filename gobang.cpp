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

    // if (point.X > 0 && Board[point.Y][point.X-1] != -player)//左非死
    // {
    //     if (Board[point.Y][point.X-1]==0 && point.X>1 && Board[point.Y][point.X-2]==player)
    //     {
    //         //if (point.X>2 && Board[point.Y][point.X-3]!= -player)
    //     }
    // }
    //逆向
    for (int i = 0; i < 5 && i < point.X; i++)
    {
        if (Board[point.Y][point.X+i] == -player)
        {
            if (i == 1)
            {
                for (int j = 0; j < 5 && j < point.X; j++)
                {
                    if (BoardState[point.Y][point.X+j][0].shape)//首个shape必然为对方的
                    {
                        if (BoardState[point.Y][point.X+j][0].shape != single)
                        {
                            BoardState[point.Y][point.X+j][0].shape =(Ownerless_Shape)((unsigned char)BoardState[point.Y][point.X+j][0].shape+1);
                        }
                    }
                }
            }
            else break;
        }
        if (Board[point.Y][point.X+i] == player)
        {
            for (int j = 0; j < 5 && j < point.X; j++)
            {
                if (BoardState[point.Y][point.X+i][0].shape)//首个shape必然为自己的
                {

                }
            }
        }

    }
}
//考虑使用读取前方格state并处理，已知问题：难以表示长连；直接在过程中修改分数，不再最终估值
void change(Point point,char dir,char player)
{
    char deltaX;
    char deltaY;
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
        if (Board[point.Y-deltaY][point.X-deltaX] == -player)
        {
            //逆向削减对方
            for (int i = 1;; i++)
            {
                if (BoardState[point.Y-i*deltaY][point.X-i*deltaX][dir].owner==-player)
                {
                    BoardState[point.Y-i*deltaY][point.X-i*deltaX][dir].shape =(Ownerless_Shape)(BoardState[point.Y-i*deltaY][point.X-i*deltaX][dir].shape+1);
                    //重构和热力图\分数修改
                    return;
                }
            }
            //正向搜索

        }
        else if (Board[point.Y-deltaY][point.X-deltaX] == player || Board[point.Y-deltaY*2][point.X-deltaX*2] == player)
        {
            for (int i = 1;; i++)
            {
                if (BoardState[point.Y-i*deltaY][point.X-i*deltaX][dir].owner==player)
                {
                    switch (BoardState[point.Y-i*deltaY][point.X-i*deltaX][dir].shape)
                    {
                        case single:
                            if (i == 1)
                            {
                                bool blocked = -Board[point.Y-2*deltaY][point.X-2*deltaX];

                            }
                            else if (i == 2)
                            {

                            }
                    }
                    //重构和热力图\分数修改
                    return;
                }
            }
        }
    }
}


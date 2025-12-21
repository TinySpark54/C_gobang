#include "gobang.h"


ChangeLog change_stack[STACK_SIZE];
ChangeLog *ptr_change_stack;
char  Board     [15][15]   ;
Shape BoardState[15][15][4];
int HeatMap   [15][15]   ;
int   Score;
Point FinalChoice;

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
int branches = MAXBRANCHES;
int search(int depth, bool isMachine, int alpha_beta) {

    if (depth>0)
    {
        int value=100000;
        if (isMachine)
        {
            value=-value;
        }
        int temp;
        Point *choices = generate_choices();
        for (int i = 0; i < branches; i++)
        {
            if (next_state(choices[i],isMachine))
            {
                de_state(choices[i]);
                if (depth==MAXDEPTH)
                {
                    FinalChoice = choices[i];
                }
                free(choices);
                return isMachine?SCORE_WIN:-SCORE_WIN;
            }
            temp = search(depth-1,!isMachine,value);

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
                    free(choices);
                    return alpha_beta;
                }
            }
            else if (!isMachine && (temp < value))
            {
                value = temp;
                if (temp <= alpha_beta)
                {
                    de_state(choices[i]);
                    free(choices);
                    return alpha_beta;
                }
            }

            de_state(choices[i]);
        }
        free(choices);
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

    int total_length;
    int total_pieces;
    int heat_temp;
    int score_temp;
    Shape shape_temp;
    char deltaX = dir?2-dir:1;
    char deltaY = dir?1:0;
    char Xrightnear = point.X + deltaX;
    char Yrightnear = point.Y + deltaY;
    bool is_alive = is_within_board(Xrightnear,Yrightnear);
    bool ismachine = player==COMPUTER;
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
                    shape_push(shape_del,{Xi,Yi},shape_temp,dir);
                    total_length = total_pieces + (shape_temp.length1 == 0 ? 0 : 1);

                    //左敌左查
                    if (total_length == i)
                    {
                        shape_temp.isblocked_end = true;
                        shape_push(shape_create, {Yi, Xi}, shape_temp, dir);
                        if (!is_alive &&is_within_board(Xrightnear,Yrightnear))
                        {
                            shape_temp=BoardState[Yrightnear][Xrightnear][dir];
                            shape_push(shape_del,{Xrightnear,Yrightnear},shape_temp,dir);
                            shape_temp.isblocked_begin = true;
                            shape_push(shape_create,{Xrightnear,Yrightnear},shape_temp,dir);
                        }
                    }
                    else//切断了敌方棋形
                    {
                        // shape_push(shape_create, {
                        //                       (char)Yi,
                        //                       (char)Xi
                        //                   },shape_temp, dir);
                        //处理前半段
                        Shape shape_left;
                        shape_left.owner = -player;
                        Shape shape_right;
                        shape_right.owner = -player;
                        if (!shape_temp.isblocked_begin)
                        {
                            if (!is_within_board(point.X-(i+2)*deltaX, point.Y-(i+2)*deltaY) ||Board[point.Y-(i+2)*deltaY][point.X-(i+2)*deltaX] != -player)
                            {
                                break_front_inline:
                                shape_left = shape_temp;
                                shape_left.length1 = 0;
                                shape_left.isblocked_end = true;
                                shape_push(shape_create,{Xi,Yi},shape_left,dir);
                            }
                            else
                            {
                                for (int j=2;;j++)
                                {
                                    char Yj = point.Y - (i+j) * deltaY;
                                    char Xj = point.X - (i+j) * deltaX;
                                    if (BoardState[Yj][Xj][dir].length0 != 0)
                                    {

                                        if (BoardState[Yj][Xj][dir].length1 ==0)
                                        {
                                            shape_left = BoardState[Yj][Xj][dir];
                                            shape_push(shape_del, {Yj,Xj},shape_left, dir);
                                            shape_left.length1=shape_temp.length0;
                                            shape_left.isblocked_end = true;
                                            shape_push(shape_create, {Yj,Xj},shape_left, dir);

                                        }
                                        else goto break_front_inline;
                                        break;
                                    }
                                }
                            }
                        }
                        //处理后半段
                        shape_right.length0 = shape_temp.length1;
                        shape_right.isblocked_begin = true;
                        //shape_temp.length1 = 0;
                        if (!shape_temp.isblocked_end)
                        {
                            char Yringt = point.Y + (shape_temp.length0+2) * deltaY;
                            char Xringt = point.X + (shape_temp.length0+2) * deltaX;
                            if (!is_within_board(Xringt, Yringt) || Board[Yringt][Xringt] != -player)
                            {
                                shape_right.length1=0;
                                shape_right.isblocked_end = false;
                                shape_push(shape_create, {(char)(point.X+1),(char)(point.Y+1)},shape_right, dir);
                            }
                            else if (BoardState[Yringt][Xringt][dir].length1 ==0)
                            {
                                shape_right.length1 = BoardState[Yringt][Xringt][dir].length0;
                                shape_right.isblocked_end = BoardState[Yringt][Xringt][dir].isblocked_end;
                                shape_push(shape_del,{Xringt,Yringt}, BoardState[Yringt][Xringt][dir], dir);
                                shape_push(shape_create,{(char)Xrightnear,(char)Yrightnear}, shape_right, dir);
                            }
                        }
                        is_alive = false;
                    }
                    break;
                }
            }
        //左敌右查
        if (is_alive)
        {
            if (Board[Yrightnear][Xrightnear] == player) //连
            {
                shape_temp = BoardState[Yrightnear][Xrightnear][dir];
                shape_push(shape_del,{Xrightnear,Yrightnear},shape_temp,dir);
                shape_temp.length0++;
                if (shape_temp.length0 >= 5)
                {
                    return true;
                }
                shape_temp.isblocked_begin == true;
            }
            else if (Board[Yrightnear][Xrightnear] == -player)
            {
                shape_temp = BoardState[Yrightnear][Xrightnear][dir];
                shape_push(shape_del,{Xrightnear,Yrightnear},shape_temp,dir);
                shape_temp.isblocked_begin = true;
                shape_push(shape_create,{Xrightnear,Yrightnear},shape_temp,dir);
                shape_temp = {ismachine,true,true,1,0};
            }
            else if (Board[Yrightnear][Xrightnear] == EMPTY)
            {
                char Xrightspace = Xrightnear + deltaX;
                char Yrightspace = Yrightnear + deltaY;
                if (is_within_board(Xrightspace,Yrightspace) && Board[Yrightspace][Xrightspace] == player && BoardState[Yrightspace][Xrightspace][dir].length1 ==0)//跳
                {
                    shape_temp = {ismachine,true,BoardState[Yrightspace][Xrightspace][dir].isblocked_end,1,BoardState[Yrightspace][Xrightspace][dir].length0};
                    shape_push(shape_del,{Xrightspace,Yrightspace},BoardState[Yrightspace][Xrightspace][dir],dir);
                }
                else
                {
                    shape_temp ={ismachine,true,false,1,0};
                }
            }
        }
        else//死
        {
            shape_temp = {ismachine,true,true,1,0};
        }
        shape_push(shape_create,point,shape_temp,dir);
        }
    }
    else if (Board[point.Y-deltaY][point.X-deltaX] == player)//左己
    {
        for (int i=1;;i++)
        {
            char Xi = point.X - i * deltaX;
            char Yi = point.Y - i * deltaY;
            if (BoardState[Yi][Xi][dir].length0 != 0)
            {
                shape_temp = BoardState[Yi][Xi][dir];
                shape_push(shape_del,{Xi,Yi},shape_temp,dir);

                if (shape_temp.length1 ==0)
                {
                    shape_temp.length0++;
                    if (shape_temp.length0 >= 5)
                    {
                        return true;
                    }
                    if (is_within_board(Xrightnear,Yrightnear))
                    {
                        char Xrightspace = Xrightnear + deltaX;
                        char Yrightspace = Yrightnear + deltaY;
                        if (Board[Yrightnear][Xrightnear] == player)
                        {
                            shape_temp.length0 += BoardState[Yrightnear][Xrightnear][dir].length0;
                            if (shape_temp.length0 >= 5)
                            {
                                return true;
                            }
                            shape_temp.length1 = BoardState[Yrightnear][Xrightnear][dir].length1;
                            shape_temp.isblocked_end = BoardState[Yrightnear][Xrightnear][dir].isblocked_end;
                            shape_push(shape_del,{Xrightnear,Yrightnear},BoardState[Yrightnear][Xrightnear][dir],dir);
                                    shape_push(shape_create,{Xi,Yi},shape_temp,dir);
                        }
                        else if (Board[Yrightnear][Xrightnear] == -player)
                        {
                            shape_temp.isblocked_end = true;
                            shape_push(shape_create,{Xi,Yi},shape_temp,dir);
                            shape_temp = BoardState[Yrightnear][Xrightnear][dir];
                            shape_push(shape_del,{Xrightnear,Yrightnear},shape_temp,dir);
                            shape_temp.isblocked_begin = true;
                            shape_push(shape_create,{Xrightnear,Yrightnear},shape_temp,dir);
                        }
                        else if (is_within_board(Xrightspace,Yrightspace)&&Board[Yrightspace][Xrightspace] == player)
                        {
                            Shape shape_right = BoardState[Yrightspace][Xrightspace][dir];
                            if (shape_right.length1 ==0)
                            {
                                shape_temp.length1 = shape_right.length0;
                                shape_temp.isblocked_end = shape_right.isblocked_end;
                                shape_push(shape_del,{Xrightspace,Yrightspace},shape_right,dir);
                                shape_push(shape_create,{Xi,Yi},shape_temp,dir);

                            }
                            else
                            {
                                shape_temp.length1 = shape_right.length0;
                                shape_push(shape_del,{Xrightspace,Yrightspace},shape_right,dir);
                                shape_right.length0 = shape_right.length1;
                                shape_right.length1 = 0;
                                shape_push(shape_create,{(char)(Xrightspace+shape_temp.length1*deltaX),(char)(Yrightspace+shape_temp.length1*deltaY)},shape_right,dir);
                                shape_push(shape_create,{Xi,Yi},shape_temp,dir);
                            }
                        }
                        else
                        {
                            shape_push(shape_create,{Xi,Yi},shape_temp,dir);
                        }
                    }
                    else
                    {
                        shape_temp.isblocked_end = true;
                        shape_push(shape_create,{Xi,Yi},shape_temp,dir);
                    }

                }
                else
                {
                    shape_temp.length1++;
                    if (shape_temp.length1 >= 5)
                    {
                        return true;
                    }
                    if (is_within_board(Xrightnear,Yrightnear))
                    {
                        if (Board[Yrightnear][Xrightnear] == -player)
                        {
                            shape_temp.isblocked_end = true;
                            shape_push(shape_create,{Xi,Yi},shape_temp,dir);
                            shape_temp = BoardState[Yrightnear][Xrightnear][dir];
                            shape_push(shape_del,{Xrightnear,Yrightnear},shape_temp,dir);
                            shape_temp.isblocked_begin = true;
                            shape_push(shape_create,{Xrightnear,Yrightnear},shape_temp,dir);
                        }
                        else if (Board[Yrightnear][Xrightnear] == player)
                        {
                            Shape shape_right = BoardState[Yrightnear][Xrightnear][dir];
                            shape_temp.length1 += shape_right.length0;
                            if (shape_temp.length1 >= 5)
                            {
                                return true;
                            }
                            shape_push(shape_del,{Xrightnear,Yrightnear},shape_right,dir);
                            shape_push(shape_create,{Xi,Yi},shape_temp,dir);
                            if (shape_right.length1 >0)
                            {
                                char Xright2 = Xrightnear+shape_right.length0*deltaX;
                                char Yright2 = Yrightnear+shape_right.length0*deltaY;
                                shape_right.length0 = shape_right.length1;
                                shape_right.length1 = 0;
                                shape_push(shape_create,{Xright2,Yright2},shape_right,dir);
                            }
                        }
                    }
                }


                shape_push(shape_create,{Xi,Yi},shape_temp,dir);
                break;
            }
        }
    }
    else if (is_within_board(point.X-deltaX*2,point.Y-deltaY*2) && Board[point.Y-deltaY*2][point.X-deltaX*2] == player)//左跳一己
    {
        for (int i=1;;i++)
        {
            char Xi = point.X - i * deltaX;
            char Yi = point.Y - i * deltaY;
            if (BoardState[Yi][Xi][dir].length0 != 0)
            {
                if (BoardState[Yi][Xi][dir].length1 == 0)
                {
                    shape_temp = BoardState[Yi][Xi][dir];
                    shape_push(shape_del,{Xi,Yi},shape_temp,dir);
                    shape_temp.length1++;
                    if (shape_temp.length1 >= 5)
                    {
                        return true;
                    }
                    if (Board[Yrightnear][Xrightnear] ==player)
                    {
                        Shape shape_right = BoardState[Yrightnear][Xrightnear][dir];
                        shape_push(shape_del,{Xrightnear,Yrightnear},shape_right,dir);
                        shape_temp.length1 += shape_right.length0;
                        if (shape_temp.length1 >= 5)
                        {
                            return true;
                        }
                        if (shape_right.length1 == 0)
                        {
                            shape_temp.isblocked_end = shape_right.isblocked_end;
                        }

                        else
                        {
                            char Xright2 = Xrightnear+shape_right.length0*deltaX;
                            char Yright2 = Yrightnear+shape_right.length0*deltaY;
                            shape_right.length0 = shape_right.length1;
                            shape_right.length1 = 0;
                            shape_push(shape_create,{Xright2,Yright2},shape_temp,dir);
                        }
                    }
                    else if (Board[Yrightnear][Xrightnear] == -player)
                    {
                        shape_temp.isblocked_end = true;
                        Shape shape_right = BoardState[Yrightnear][Xrightnear][dir];
                        shape_push(shape_del,{Xrightnear,Yrightnear},shape_right,dir);
                        shape_right.isblocked_begin = true;
                        shape_push(shape_create,{Xrightnear,Yrightnear},shape_right,dir);
                    }
                    shape_push(shape_create,{Xi,Yi},shape_temp,dir);
                }
                else
                {
                    goto leftempty;
                }
                break;
            }
        }
    }
    else//孤子
    {
        leftempty:
        if (is_within_board(Xrightnear,Yrightnear))
        {
            char Xrightspace = Xrightnear + deltaX;
            char Yrightspace = Yrightnear + deltaY;
            if (Board[Yrightnear][Xrightnear] == player)
            {
                shape_temp = BoardState[Yrightnear][Xrightnear][dir];
                shape_push(shape_del,{Xrightnear,Yrightnear},shape_temp,dir);
                shape_temp.length0++;
                if (shape_temp.length0 >= 5)
                {
                    return true;
                }
                shape_push(shape_create,point,shape_temp,dir);
            }
            else if (Board[Yrightnear][Xrightnear] == -player)
            {
                shape_temp = {ismachine,false,true,1,0};
                Shape shape_right = BoardState[Yrightnear][Xrightnear][dir];
                shape_push(shape_del,{Xrightnear,Yrightnear},shape_right,dir);
                shape_right.isblocked_begin = true;
                shape_push(shape_create,{Xrightnear,Yrightnear},shape_right,dir);
            }
            else if (is_within_board(Xrightspace,Yrightspace)&&Board[Yrightspace][Xrightspace]==player&&BoardState[Yrightspace][Xrightspace][dir].length1 == 0)
            {
                shape_temp = BoardState[Yrightspace][Xrightspace][dir];
                shape_push(shape_del,{Xrightspace,Yrightspace},shape_temp,dir);
                shape_temp.length1 = shape_temp.length0;
                shape_temp.length0 = 1;

            }
            else
            {
                shape_temp = {ismachine,false,false,1,0};
            }
            shape_push(shape_create,point,shape_temp,dir);
        }
        else
        {
            shape_push(shape_create,point,{ismachine,false,true,1,0},dir);
        }
        //改 owner与player的判定
        //中途胜利判定
        //右判
    }
    return false;

}
void de_state(Point point) {
    Board[point.Y][point.X] = EMPTY;
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
            case shape_del:
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
    branches = step;
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
//是否减少jump棋形以降低case数？不用了，没有棋形枚举了已经
//改变棋形表示方法，两位分别表示两头死活，**内部直接用对应二进制**(改为断点前后两段长度，连跳直接拆分)，再找规律直接加加减减？
//先不搞禁手了

bool is_within_board(char X, char Y)
{
    return X>=0 && X<BOARD_SIZE && Y>=0 && Y<BOARD_SIZE;
}
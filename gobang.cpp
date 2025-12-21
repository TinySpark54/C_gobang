#include "gobang.h"

#include <cstdio>

int main()
{
    int X_human;
    int Y_human;
    bool first_run;
    int computer_score;
    bool computer_win = false;
    bool human_win = false;
    stack_init();
    printf_s("1：电脑先行，0：玩家先行：");
    scanf_s("%d",&first_run);

    if(first_run)
    {
        next_state({7,7},true);
        stack_init();
        printf_s("电脑下在了（7，7）\n");
    }
    while(true)
    {
        human:
        printf_s("请输入坐标x：");
        scanf_s("%d",&X_human,1);
        printf_s("请输入坐标y：");
        scanf_s("%d",&Y_human,1);
        if (!is_within_board(X_human,Y_human) || Board[Y_human][X_human]!= EMPTY)
        {
            printf_s("错误");
            goto human;
        }
        human_win = next_state({(char)X_human,(char)Y_human},false);
        if(human_win)
        {
            printf_s("玩家获胜");
            break;
        }
        stack_init();

        computer_score = search(MAXDEPTH,true,-100000);
        printf_s("%d",computer_score);
        computer_win = (computer_score>=SCORE_WIN);
        next_state(FinalChoice,true);
        printf_s("电脑下在了（%d，%d）",FinalChoice.X,FinalChoice.Y);
        if(computer_win)
        {
            printf_s("电脑获胜");
            break;
        }
    }
    system("pause");
    return 0;
}

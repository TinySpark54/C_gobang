#include "gobang.h"
#include <cstdio>

int main()
{
    int X_human;
    int Y_human;
    int first_run;
    bool computer_win = false;
    bool human_win = false;

    stack_init();
    init_board();

    printf("五子棋游戏开始！\n");
    printf("1：电脑先行，0：玩家先行：");
    scanf("%d", &first_run);

    if(first_run)
    {
        next_state({7,7}, true);
        stack_init();
        printf("电脑下在了（7，7）\n");
    }

    // 打印棋盘
    printf("当前棋盘：\n");
    for(int i = 0; i < BOARD_SIZE; i++)
    {
        for(int j = 0; j < BOARD_SIZE; j++)
        {
            if(Board[i][j] == COMPUTER) printf("X ");
            else if(Board[i][j] == HUMAN) printf("O ");
            else printf(". ");
        }
        printf("\n");
    }

    while(true)
    {
        human:
        printf("\n请输入坐标x（0-14）：");
        scanf("%d", &X_human);
        printf("请输入坐标y（0-14）：");
        scanf("%d", &Y_human);

        if (!is_within_board(X_human, Y_human) || Board[Y_human][X_human] != EMPTY)
        {
            printf("坐标无效或该位置已有棋子，请重新输入！\n");
            goto human;
        }

        human_win = next_state({(char)X_human, (char)Y_human}, false);
        stack_init();

        if(human_win)
        {
            printf("\n玩家获胜！\n");

            // 打印最终棋盘
            printf("最终棋盘：\n");
            for(int i = 0; i < BOARD_SIZE; i++)
            {
                for(int j = 0; j < BOARD_SIZE; j++)
                {
                    if(Board[i][j] == COMPUTER) printf("X ");
                    else if(Board[i][j] == HUMAN) printf("O ");
                    else printf(". ");
                }
                printf("\n");
            }
            break;
        }

        // 电脑下棋
        printf("\n电脑思考中...\n");
        int computer_score = search(MAXDEPTH, true, -1000000);
        computer_win = (computer_score >= SCORE_WIN);

        if(computer_win)
        {
            next_state(FinalChoice, true);
            printf("电脑下在了（%d，%d）\n", FinalChoice.X, FinalChoice.Y);
            printf("\n电脑获胜！\n");

            // 打印最终棋盘
            printf("最终棋盘：\n");
            for(int i = 0; i < BOARD_SIZE; i++)
            {
                for(int j = 0; j < BOARD_SIZE; j++)
                {
                    if(Board[i][j] == COMPUTER) printf("X ");
                    else if(Board[i][j] == HUMAN) printf("O ");
                    else printf(". ");
                }
                printf("\n");
            }
            break;
        }

        next_state(FinalChoice, true);
        printf("电脑下在了（%d，%d）\n", FinalChoice.X, FinalChoice.Y);
        stack_init();

        // 打印棋盘
        printf("\n当前棋盘：\n");
        for(int i = 0; i < BOARD_SIZE; i++)
        {
            for(int j = 0; j < BOARD_SIZE; j++)
            {
                if(Board[i][j] == COMPUTER) printf("X ");
                else if(Board[i][j] == HUMAN) printf("O ");
                else printf(". ");
            }
            printf("\n");
        }
    }

    system("pause");
    return 0;
}
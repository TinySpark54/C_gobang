#include <graphics.h>
#include <conio.h>

int main()
{
    initgraph(640, 480);

    setbkcolor(RGB(135, 206, 235));
    cleardevice();

    settextcolor(RGB(0, 0, 255));
    settextstyle(30, 0, _T("宋体"));
    outtextxy(200, 400, _T("Hello World"));

    _getch();
    closegraph();

    return 0;
}
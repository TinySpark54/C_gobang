#include "gobangboard.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 设置全局字体
    QFont font("Microsoft YaHei", 10);
    app.setFont(font);
    
    // 创建并显示主窗口
    GobangBoard board;
    board.show();
    
    return app.exec();
}
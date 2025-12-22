#ifndef GOBANGBOARD_H
#define GOBANGBOARD_H

#include "gobang.h"
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTimer>

class GobangBoard : public QWidget
{
    Q_OBJECT

public:
    explicit GobangBoard(QWidget *parent = nullptr);
    ~GobangBoard();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void newGame();
    void computerFirst();
    void playerFirst();
    void computerMove();
    void showAbout();

private:
    bool checkWin(int x, int y, char player);
    void drawBoard(QPainter &painter);
    void drawPieces(QPainter &painter);
    void drawLastMove(QPainter &painter);
    void updateStatus();
    QPoint boardToScreen(int boardX, int boardY);
    QPoint screenToBoard(QPoint screenPos);
    void gameOver(bool computerWins);

    // 界面组件
    QPushButton *btnNewGame;
    QPushButton *btnComputerFirst;
    QPushButton *btnPlayerFirst;
    QPushButton *btnAbout;
    QLabel *lblStatus;
    QLabel *lblScore;

    // 布局
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    QWidget *controlWidget;

    // 游戏状态
    bool gameStarted;
    bool computerTurn;
    bool gameOverFlag;
    Point lastMove;
    int gridSize;
    int boardMargin;
    QRect boardRect;

    // 计时器用于延迟电脑落子
    QTimer *computerTimer;
};

#endif // GOBANGBOARD_H

#include "gobangboard.h"
#include <cmath>
#include <QDebug>

GobangBoard::GobangBoard(QWidget *parent)
    : QWidget(parent)
    , gameStarted(false)
    , computerTurn(false)
    , gameOverFlag(false)
    , gridSize(35)
    , boardMargin(40)
{
    // 初始化游戏状态
    stack_init();
    init_board();

    // 创建界面组件
    btnNewGame = new QPushButton("新游戏");
    btnComputerFirst = new QPushButton("电脑先手");
    btnPlayerFirst = new QPushButton("玩家先手");
    btnAbout = new QPushButton("关于");
    lblStatus = new QLabel("请选择先手方开始游戏");
    lblScore = new QLabel("得分: 0");

    // 设置组件样式
    btnNewGame->setFixedSize(100, 30);
    btnComputerFirst->setFixedSize(100, 30);
    btnPlayerFirst->setFixedSize(100, 30);
    btnAbout->setFixedSize(100, 30);
    lblStatus->setFixedHeight(30);
    lblStatus->setStyleSheet("font-weight: bold; font-size: 14px;");
    lblScore->setFixedHeight(30);

    // 创建控制面板
    controlWidget = new QWidget();
    QVBoxLayout *controlLayout = new QVBoxLayout(controlWidget);
    buttonLayout = new QHBoxLayout();

    buttonLayout->addWidget(btnNewGame);
    buttonLayout->addWidget(btnComputerFirst);
    buttonLayout->addWidget(btnPlayerFirst);
    buttonLayout->addWidget(btnAbout);
    buttonLayout->addStretch();

    controlLayout->addLayout(buttonLayout);
    controlLayout->addWidget(lblStatus);
    controlLayout->addWidget(lblScore);
    controlWidget->setFixedHeight(120);

    // 主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(controlWidget);
    mainLayout->addStretch(1); // 棋盘区域会占用剩余空间

    // 连接信号槽
    connect(btnNewGame, &QPushButton::clicked, this, &GobangBoard::newGame);
    connect(btnComputerFirst, &QPushButton::clicked, this, &GobangBoard::computerFirst);
    connect(btnPlayerFirst, &QPushButton::clicked, this, &GobangBoard::playerFirst);
    connect(btnAbout, &QPushButton::clicked, this, &GobangBoard::showAbout);

    // 创建电脑思考计时器
    computerTimer = new QTimer(this);
    computerTimer->setSingleShot(true);
    connect(computerTimer, &QTimer::timeout, this, &GobangBoard::computerMove);

    // 设置窗口属性
    setWindowTitle("五子棋游戏");
    setMinimumSize(700, 750);

    // 设置样式
    setStyleSheet(R"(
        QWidget {
            background-color: #F5F5DC;
        }
        QPushButton {
            background-color: #8B4513;
            color: white;
            border: 2px solid #654321;
            border-radius: 5px;
            padding: 5px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #A0522D;
        }
        QPushButton:pressed {
            background-color: #654321;
        }
        QLabel {
            color: #8B4513;
        }
    )");
}

GobangBoard::~GobangBoard()
{
}

void GobangBoard::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 计算棋盘区域
    int controlHeight = controlWidget->height();
    int availableHeight = height() - controlHeight - 20;
    int availableWidth = width() - 20;

    int boardSize = qMin(availableHeight, availableWidth);
    boardMargin = qMax(20, (availableWidth - boardSize) / 2);
    int topMargin = controlHeight + 10 + qMax(0, (availableHeight - boardSize) / 2);

    boardRect = QRect(boardMargin, topMargin, boardSize, boardSize);

    // 绘制棋盘背景
    painter.fillRect(boardRect, QColor("#DEB887"));

    // 绘制棋盘边框
    painter.setPen(QPen(QColor("#8B4513"), 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(boardRect);

    // 绘制棋盘网格和棋子
    drawBoard(painter);
    drawPieces(painter);

    // 高亮显示最后一步
    if (gameStarted && !gameOverFlag && lastMove.X != -1 && lastMove.Y != -1)
    {
        drawLastMove(painter);
    }
}

void GobangBoard::drawBoard(QPainter &painter)
{
    // 计算棋盘内部绘制区域
    int innerMargin = 20;
    int innerSize = boardRect.width() - 2 * innerMargin;
    gridSize = innerSize / (BOARD_SIZE - 1);

    // 限制gridSize在合理范围内
    if (gridSize < 20) {
        gridSize = 20;
        innerSize = gridSize * (BOARD_SIZE - 1);
        innerMargin = (boardRect.width() - innerSize) / 2;
    }

    int startX = boardRect.x() + innerMargin;
    int startY = boardRect.y() + innerMargin;

    // 绘制网格线
    painter.setPen(QPen(QColor("#8B4513"), 2));

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        // 横线
        int y = startY + i * gridSize;
        painter.drawLine(startX, y, startX + innerSize, y);

        // 竖线
        int x = startX + i * gridSize;
        painter.drawLine(x, startY, x, startY + innerSize);
    }

    // 绘制棋盘上的点（天元和星位）
    painter.setBrush(QBrush(QColor("#8B4513")));

    // 天元 (7,7)
    int centerX = startX + 7 * gridSize;
    int centerY = startY + 7 * gridSize;
    painter.drawEllipse(QPoint(centerX, centerY), 5, 5);

    // 四个星位
    painter.drawEllipse(QPoint(startX + 3 * gridSize, startY + 3 * gridSize), 4, 4);
    painter.drawEllipse(QPoint(startX + 11 * gridSize, startY + 3 * gridSize), 4, 4);
    painter.drawEllipse(QPoint(startX + 3 * gridSize, startY + 11 * gridSize), 4, 4);
    painter.drawEllipse(QPoint(startX + 11 * gridSize, startY + 11 * gridSize), 4, 4);
}

void GobangBoard::drawPieces(QPainter &painter)
{
    // 计算棋盘内部绘制区域（与drawBoard一致）
    int innerMargin = 20;
    int innerSize = boardRect.width() - 2 * innerMargin;
    gridSize = innerSize / (BOARD_SIZE - 1);

    if (gridSize < 20) {
        gridSize = 20;
        innerSize = gridSize * (BOARD_SIZE - 1);
        innerMargin = (boardRect.width() - innerSize) / 2;
    }

    int startX = boardRect.x() + innerMargin;
    int startY = boardRect.y() + innerMargin;
    int pieceRadius = gridSize / 2 - 2;

    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (Board[y][x] != EMPTY)
            {
                QPoint center(startX + x * gridSize, startY + y * gridSize);

                // 绘制棋子阴影（3D效果）
                painter.setPen(Qt::NoPen);
                if (Board[y][x] == COMPUTER)
                {
                    // 电脑棋子（黑色）
                    QRadialGradient gradient(center, pieceRadius);
                    gradient.setColorAt(0, QColor(100, 100, 100));
                    gradient.setColorAt(0.7, QColor(50, 50, 50));
                    gradient.setColorAt(1, QColor(20, 20, 20));
                    painter.setBrush(gradient);
                }
                else
                {
                    // 玩家棋子（白色）
                    QRadialGradient gradient(center, pieceRadius);
                    gradient.setColorAt(0, QColor(255, 255, 255));
                    gradient.setColorAt(0.7, QColor(230, 230, 230));
                    gradient.setColorAt(1, QColor(200, 200, 200));
                    painter.setBrush(gradient);
                }

                painter.drawEllipse(center, pieceRadius, pieceRadius);

                // 绘制棋子边框
                painter.setPen(QPen(QColor(100, 100, 100), 1));
                painter.setBrush(Qt::NoBrush);
                painter.drawEllipse(center, pieceRadius, pieceRadius);
            }
        }
    }
}

void GobangBoard::drawLastMove(QPainter &painter)
{
    if (lastMove.X == -1 && lastMove.Y == -1) return;

    // 计算棋盘内部绘制区域（与drawBoard一致）
    int innerMargin = 20;
    int innerSize = boardRect.width() - 2 * innerMargin;
    gridSize = innerSize / (BOARD_SIZE - 1);

    if (gridSize < 20) {
        gridSize = 20;
        innerSize = gridSize * (BOARD_SIZE - 1);
        innerMargin = (boardRect.width() - innerSize) / 2;
    }

    int startX = boardRect.x() + innerMargin;
    int startY = boardRect.y() + innerMargin;

    QPoint center(startX + lastMove.X * gridSize, startY + lastMove.Y * gridSize);

    // 绘制红色圆圈标记最后一步
    painter.setPen(QPen(QColor(255, 0, 0, 200), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(center, gridSize / 3, gridSize / 3);
}

QPoint GobangBoard::screenToBoard(QPoint screenPos)
{
    if (!boardRect.contains(screenPos))
        return QPoint(-1, -1);

    // 计算棋盘内部绘制区域（与drawBoard一致）
    int innerMargin = 20;
    int innerSize = boardRect.width() - 2 * innerMargin;
    gridSize = innerSize / (BOARD_SIZE - 1);

    if (gridSize < 20) {
        gridSize = 20;
        innerSize = gridSize * (BOARD_SIZE - 1);
        innerMargin = (boardRect.width() - innerSize) / 2;
    }

    int startX = boardRect.x() + innerMargin;
    int startY = boardRect.y() + innerMargin;

    float boardX = (screenPos.x() - startX) / (float)gridSize;
    float boardY = (screenPos.y() - startY) / (float)gridSize;

    // 四舍五入到最近的整数
    int roundedX = round(boardX);
    int roundedY = round(boardY);

    // 确保在棋盘范围内
    if (roundedX < 0) roundedX = 0;
    if (roundedX >= BOARD_SIZE) roundedX = BOARD_SIZE - 1;
    if (roundedY < 0) roundedY = 0;
    if (roundedY >= BOARD_SIZE) roundedY = BOARD_SIZE - 1;

    // 检查点击位置是否靠近交叉点（允许一定的误差范围）
    float diffX = fabs(boardX - roundedX);
    float diffY = fabs(boardY - roundedY);

    // 如果点击位置距离交叉点太远，返回无效点
    if (diffX > 0.3 || diffY > 0.3)
    {
        return QPoint(-1, -1);
    }

    return QPoint(roundedX, roundedY);
}

void GobangBoard::mousePressEvent(QMouseEvent *event)
{
    if (!gameStarted || gameOverFlag || computerTurn)
        return;

    QPoint clickPos = event->pos();

    // 检查是否点击在棋盘区域内
    if (!boardRect.contains(clickPos))
        return;

    QPoint boardPos = screenToBoard(clickPos);
    int x = boardPos.x();
    int y = boardPos.y();

    // 检查是否有效点击
    if (x == -1 || y == -1)
        return;

    // 检查位置是否为空
    if (Board[y][x] == EMPTY)
    {
        qDebug() << "玩家下棋位置: (" << x << "," << y << ")";

        // 检查是否获胜
        bool humanWin = checkWin(x, y, HUMAN);

        // 玩家走棋
        if (humanWin)
        {
            Board[y][x] = HUMAN;
            lastMove = {(char)x, (char)y};
            gameOver(false);
            update();
            return;
        }

        // 使用原来的状态更新
        bool immediateWin = next_state({(char)x, (char)y}, false);
        stack_init();

        lastMove = {(char)x, (char)y};

        if (immediateWin)
        {
            gameOver(false);
            update();
            return;
        }

        // 轮到电脑
        computerTurn = true;
        updateStatus();

        // 延迟电脑思考，给用户反应时间
        computerTimer->start(500);

        update();
    }
}

void GobangBoard::computerMove()
{
    if (!gameStarted || gameOverFlag || !computerTurn)
        return;

    qDebug() << "电脑思考中...";

    // 电脑走棋
    search(MAXDEPTH, true, -1000000);

    // 电脑下棋
    next_state(FinalChoice, true);
    lastMove = FinalChoice;
    stack_init();

    qDebug() << "电脑下棋位置: (" << FinalChoice.X << "," << FinalChoice.Y << ")";

    // 检查电脑是否获胜
    bool computerWin = checkWin(FinalChoice.X, FinalChoice.Y, COMPUTER);
    if (computerWin)
    {
        update();
        gameOver(true);
        update();
        return;
    }

    computerTurn = false;
    updateStatus();
    update();
}

void GobangBoard::newGame()
{
    qDebug() << "新游戏";

    // 重置游戏状态
    init_board();
    stack_init();
    gameStarted = false;
    gameOverFlag = false;
    computerTurn = false;
    lastMove = {-1, -1};

    lblStatus->setText("请选择先手方开始游戏");
    lblScore->setText("得分: 0");

    update();
}

void GobangBoard::computerFirst()
{
    qDebug() << "电脑先手";
    newGame();
    gameStarted = true;
    computerTurn = true;
    lblStatus->setText("游戏开始 - 电脑先手");

    // 电脑走第一步
    computerTimer->start(100);
}

void GobangBoard::playerFirst()
{
    qDebug() << "玩家先手";
    newGame();
    gameStarted = true;
    computerTurn = false;
    lblStatus->setText("游戏开始 - 玩家先手");
}

void GobangBoard::updateStatus()
{
    if (!gameStarted)
        return;

    QString status;
    if (gameOverFlag)
    {
        status = "游戏结束";
    }
    else if (computerTurn)
    {
        status = "电脑思考中...";
    }
    else
    {
        status = "轮到玩家下棋";
    }

    lblStatus->setText(status);
    lblScore->setText(QString("得分: %1").arg(Score));
}

void GobangBoard::gameOver(bool computerWins)
{
    gameOverFlag = true;
    gameStarted = false;

    QString message;
    if (computerWins)
    {
        message = "电脑获胜！";
    }
    else
    {
        message = "恭喜你获胜！";
    }

    QMessageBox::information(this, "游戏结束", message);
    lblStatus->setText("游戏结束 - " + message);
}

void GobangBoard::showAbout()
{
    QMessageBox::about(this, "关于五子棋",
                       "五子棋游戏\n\n"
                       "规则说明：\n"
                       "1. 在15×15的棋盘上进行游戏\n"
                       "2. 黑白双方轮流落子\n"
                       "3. 首先在横、竖、斜方向连成五子者获胜\n\n"
                       "操作说明：\n"
                       "1. 点击棋盘交叉点放置棋子\n"
                       "2. 使用按钮控制游戏流程\n\n"
                       "技术特点：\n"
                       "• 使用Alpha-Beta剪枝搜索算法\n"
                       "• 支持热力图启发式搜索\n"
                       "• 美观的图形界面\n\n"
                       "作者：TinySpark\n"
                       "版本：0.1");
}

void GobangBoard::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    update(); // 触发重绘
}
bool GobangBoard::checkWin(int x, int y, char player)
{
    // 检查八个方向（四个直线方向）
    int directions[4][2] = {
        {1, 0},  // 水平
        {0, 1},  // 垂直
        {1, 1},  // 主对角线
        {1, -1}  // 副对角线
    };

    for (int d = 0; d < 4; d++)
    {
        int dx = directions[d][0];
        int dy = directions[d][1];
        int count = 1; // 当前位置的棋子

        // 正向检查
        for (int i = 1; i <= 4; i++)
        {
            int newX = x + i * dx;
            int newY = y + i * dy;

            if (newX < 0 || newX >= BOARD_SIZE ||
                newY < 0 || newY >= BOARD_SIZE ||
                Board[newY][newX] != player)
                break;

            count++;
        }

        // 反向检查
        for (int i = 1; i <= 4; i++)
        {
            int newX = x - i * dx;
            int newY = y - i * dy;

            if (newX < 0 || newX >= BOARD_SIZE ||
                newY < 0 || newY >= BOARD_SIZE ||
                Board[newY][newX] != player)
                break;

            count++;
        }

        // 如果连续5个或以上
        if (count >= 5)
            return true;
    }

    return false;
}

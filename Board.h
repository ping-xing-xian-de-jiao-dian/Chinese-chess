#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include "Chess.h"

class Board : public QWidget
{
    Q_OBJECT
public:
    explicit Board(QWidget *parent = nullptr);
    ~Board();

    // 32个棋子
    Chess m_chess[32];
    // 棋盘的这个位置有没有棋子，如果有，存放id，没有则是-1
    int m_hasChess[10][9];
    // 格子的大小
    int m_gridSize;
    // 棋子的半径
    int m_chessR;
    // 象棋上的字体大小
    int m_wordSize;
    // 选中的棋子
    int m_selectId;
    // 是否轮到红棋下
    bool m_redTurn;
    // 红棋在下面
    bool m_redUnder;
    // 点击位置
    QPoint m_clkPos;



    // 绘制棋盘
    void paintEvent(QPaintEvent* );

    // 绘制棋子
    void drawChess(QPainter&, int);

    // 执红旗黑棋初始化
    void initRedBlack(bool);

    // 根据棋子id或row和col把row和col转换为像素坐标
    QPoint rowcol2pixal(int);
    QPoint rowcol2pixal(int, int);

    // 鼠标点击，走子
    void mouseReleaseEvent(QMouseEvent* );

    // 判断离点击点最近的交叉点，如果没点到，返回false
    bool getRowCol(QPoint, int&, int&);

    // 实现点击，选中，走子，吃子等
    virtual void click(int, int, int);

    // 实现走棋规则
    bool canMove(int, int, int, int);
    bool canMoveJiang(int, int, int, int);
    bool canMoveShi(int, int, int, int);
    bool canMoveXiang(int, int, int, int);
    bool canMoveJu(int, int, int, int);
    bool canMoveMa(int, int, int, int);
    bool canMovePao(int, int, int, int);
    bool canMoveBing(int, int, int, int);

    // 是否是将军状态
    bool jiangJun(int, int, int, int);





signals:

public slots:
};

#endif // BOARD_H

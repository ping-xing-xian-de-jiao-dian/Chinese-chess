#include "Board.h"
#include <QPainter>
#include <QRect>
#include <QMouseEvent>
#include <QVector>

// 构造函数初始化
Board::Board(QWidget *parent) : QWidget(parent){
    // 绘制棋盘参数初始化
    m_gridSize = 80;
    m_chessR = 36;
    m_wordSize = 32;
    m_selectId = -1;
    m_redTurn = true;
    for (int i = 0; i < 10; ++i){
        for (int j = 0; j < 9; ++j){
            m_hasChess[i][j] = -1;
        }
    }

    // 32个棋子初始化
    for (int id = 0; id < 32; ++id){
        m_chess[id].init(id);
        m_hasChess[m_chess[id].m_row][m_chess[id].m_col] = id;
    }
}



// 绘制棋盘
void Board::paintEvent(QPaintEvent *){
    QPainter painter(this);
    QPen pen;

    //画边框
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawLine(QPoint(m_gridSize, m_gridSize), QPoint(9 * m_gridSize, m_gridSize));
    painter.drawLine(QPoint(m_gridSize, 10 * m_gridSize), QPoint(9 * m_gridSize, 10 * m_gridSize));
    painter.drawLine(QPoint(m_gridSize, m_gridSize), QPoint(m_gridSize, 10 * m_gridSize));
    painter.drawLine(QPoint(9 * m_gridSize, m_gridSize), QPoint(9 * m_gridSize, 10 * m_gridSize));
    pen.setWidth(1);
    painter.setPen(pen);
    // 画横线
    for (int i = 1; i < 9; ++i){
        painter.drawLine(QPoint(m_gridSize, m_gridSize + i * m_gridSize), QPoint(9 * m_gridSize, m_gridSize + i * m_gridSize));
    }
    // 画竖线
    for (int j = 1; j < 8; ++j){
        painter.drawLine(QPoint(m_gridSize + j * m_gridSize, m_gridSize), QPoint(m_gridSize + j * m_gridSize, 5 * m_gridSize));
        painter.drawLine(QPoint(m_gridSize + j * m_gridSize, 6 * m_gridSize), QPoint(m_gridSize + j * m_gridSize, 10 * m_gridSize));
    }
    // 画斜线
    painter.drawLine(QPoint(4 * m_gridSize, m_gridSize), QPoint(6 * m_gridSize, 3 * m_gridSize));
    painter.drawLine(QPoint(6 * m_gridSize, m_gridSize), QPoint(4 * m_gridSize, 3 * m_gridSize));
    painter.drawLine(QPoint(4 * m_gridSize, 8 * m_gridSize), QPoint(6 * m_gridSize, 10 * m_gridSize));
    painter.drawLine(QPoint(6 * m_gridSize, 8 * m_gridSize), QPoint(4 * m_gridSize, 10 * m_gridSize));

    // 画棋子
    for (int id = 0; id < 32; ++id){
        drawChess(painter, id);
    }
}




// 绘制棋子
void Board::drawChess(QPainter &painter, int id){
    // 如果死了，就不画了
    if (m_chess[id].m_dead) return;

    // 画棋的边框，加字体色背景色
    painter.setBrush(QBrush(QColor(227, 170, 79)));
    // 选中棋子时，背景色变化
    if (m_selectId == id) painter.setBrush(QBrush(QColor(255, 255, 0)));

    painter.setPen(Qt::black);
    painter.drawEllipse(rowcol2pixal(id), m_chessR, m_chessR);
    if (m_chess[id].m_red) painter.setPen(Qt::red);

    // 确定字的位置，字体设置
    QPoint c = rowcol2pixal(id);
    QRect rect(c.x() - m_chessR, c.y() - m_chessR, m_chessR * 2, m_chessR * 2);
    painter.setFont(QFont("楷体", m_wordSize, 700));
    painter.drawText(rect, m_chess[id].getText(), QTextOption(Qt::AlignCenter));
}



// 根据棋子id把row和col转换为像素坐标
QPoint Board::rowcol2pixal(int id){
    int row = m_chess[id].m_row;
    int col = m_chess[id].m_col;
    QPoint res((col + 1) * m_gridSize, (row + 1) * m_gridSize);
    return res;
}
// 根据棋子row和col转换为像素坐标
QPoint Board::rowcol2pixal(int row, int col){
    QPoint res((col + 1) * m_gridSize, (row + 1) * m_gridSize);
    return res;
}



// 判断离点击点最近的交叉点，如果没点到，返回false
bool Board::getRowCol(QPoint pt, int &row, int &col){
    for (row = 0; row <= 9; ++row){
        for (col = 0; col <= 8; ++col){
            QPoint c = rowcol2pixal(row, col);
            int dx = c.x() - pt.x();
            int dy = c.y() - pt.y();
            int dis = dx * dx + dy * dy;
            if (dis <= m_chessR * m_chessR) return true;
        }
    }
    return false;
}


// 将的移动规则
bool Board::canMoveJiang(int selectId, int clkRow, int clkCol, int clkId){
    int row = m_chess[selectId].m_row;
    int col = m_chess[selectId].m_col;
    // 范围在九宫格
    if (clkCol < 3 || clkCol > 5) return false;
    if (m_chess[selectId].m_red){
        if (clkRow < 0 || clkRow > 2) return false;
    } else {
        if (clkRow < 7 || clkRow > 9) return false;
    }

    // 只能上下左右移动一格
    QVector<QVector<int>> dirs = {
        {0, -1}, {0, 1}, {1, 0}, {-1, 0}
    };
    for (auto dir : dirs){
        if (row + dir[0] == clkRow && col + dir[1] == clkCol){
            return true;
        }
    }

    return false;
}

// 士的移动规则
bool Board::canMoveShi(int selectId, int clkRow, int clkCol, int clkId){
    int row = m_chess[selectId].m_row;
    int col = m_chess[selectId].m_col;
    // 范围在九宫格
    if (clkCol < 3 || clkCol > 5) return false;
    if (m_chess[selectId].m_red){
        if (clkRow < 0 || clkRow > 2) return false;
    } else {
        if (clkRow < 7 || clkRow > 9) return false;
    }

    // 只能对角线移动
    QVector<QVector<int>> dirs = {
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };
    for (auto dir : dirs){
        if (row + dir[0] == clkRow && col + dir[1] == clkCol){
            return true;
        }
    }

    return false;
}

// 象的移动规则
bool Board::canMoveXiang(int selectId, int clkRow, int clkCol, int clkId){
    // 范围在己方
    if (m_chess[selectId].m_red){
        if (clkRow < 0 || clkRow > 4) return false;
    } else {
        if (clkRow < 5 || clkRow > 9) return false;
    }

    // 田字格移动，不能被卡象眼
    QVector<QVector<int>> dirs = {
        {-2, -2}, {-2, 2}, {2, -2}, {2, 2}
    };
    for (auto dir : dirs){
        int rrow = m_chess[selectId].m_row + dir[0];
        int ccol = m_chess[selectId].m_col + dir[1];
        if (rrow == clkRow && ccol == clkCol){
            // 卡象眼了
            if (m_hasChess[rrow - dir[0] / 2][ccol - dir[1] / 2] != -1) return false;
            return true;
        }
    }

    return false;
}

// 車的移动规则
bool Board::canMoveJu(int selectId, int clkRow, int clkCol, int clkId){
    // 走直线，不能越过别的子
    int row = m_chess[selectId].m_row;
    int col = m_chess[selectId].m_col;
    if (col == clkCol){
        if (row > clkRow){
            int temp = row; row = clkRow; clkRow = temp;
        }
        for (int i = row + 1; i < clkRow; ++i){
            if (m_hasChess[i][col] != -1) return false;
        }
    } else if (row == clkRow) {
        if (col > clkCol){
            int temp = col; col = clkCol; clkCol = temp;
        }
        for (int j = col + 1; j < clkCol; ++j){
            if (m_hasChess[row][j] != -1) return false;
        }
    } else {
        return false;
    }

    return true;
}

// 马的移动规则
bool Board::canMoveMa(int selectId, int clkRow, int clkCol, int clkId){
    // 日字格移动，不能被别马脚
    int row = m_chess[selectId].m_row;
    int col = m_chess[selectId].m_col;
    // 分别表示x，y轴移动方向和别马脚的格子
    QVector<QVector<int>> dirs = {
        {-1, 2, 0, 1}, {-2, 1, -1, 0}, {2, 1, 1, 0}, {-1, -2, 0, -1},
        {-2, -1, -1, 0}, {2, -1, 1, 0}, {1, -2, 0, -1}, {1, 2, 0, 1}
    };
    for (auto dir : dirs){
        if (row + dir[0] == clkRow && col + dir[1] == clkCol){
            if (m_hasChess[row + dir[2]][col + dir[3]] != -1) return false;
            return true;
        }
    }

    return false;
}

// 炮的移动规则
bool Board::canMovePao(int selectId, int clkRow, int clkCol, int clkId){
    // 吃子能越过一个子，不吃不行
    int row = m_chess[selectId].m_row;
    int col = m_chess[selectId].m_col;
    if (m_hasChess[clkRow][clkCol] == -1){
        return canMoveJu(selectId, clkRow, clkCol, clkId);
    } else {
        int count = 0;
        if (col == clkCol){
            if (row > clkRow){
                int temp = row; row = clkRow; clkRow = temp;
            }
            for (int i = row + 1; i < clkRow; ++i){
                if (m_hasChess[i][col] != -1) ++count;
            }
        } else if (row == clkRow) {
            if (col > clkCol){
                int temp = col; col = clkCol; clkCol = temp;
            }
            for (int j = col + 1; j < clkCol; ++j){
                if (m_hasChess[row][j] != -1) ++count;
            }
        } else {
            return false;
        }
        if (count == 1) return true;
        return false;
    }
}

// 兵的移动规则
bool Board::canMoveBing(int selectId, int clkRow, int clkCol, int clkId){
    int row = m_chess[selectId].m_row;
    int col = m_chess[selectId].m_col;
    // 己方只能往前一步一步走，过河能左右前
    if (m_chess[selectId].m_red){
        if (clkRow < 6){
            if (clkRow == row + 1 && clkCol == col) return true;
        } else {
            QVector<QVector<int>> dirs = {
                {1, 0}, {0, 1}, {0, -1}
            };
            for (auto dir : dirs){
                if (row + dir[0] == clkRow && col + dir[1] == clkCol){
                    return true;
                }
            }
        }
    } else {
        if (clkRow > 4){
            if (clkRow == row - 1 && clkCol == col) return true;
        } else {
            QVector<QVector<int>> dirs = {
                {-1, 0}, {0, 1}, {0, -1}
            };
            for (auto dir : dirs){
                if (row + dir[0] == clkRow && col + dir[1] == clkCol){
                    return true;
                }
            }
        }
    }

    return false;
}



// 实现走棋规则，这个棋子能不能动
bool Board::canMove(int selectId, int clkRow, int clkCol, int clkId){
    // 如果是自己，不能吃，而是取消选择
    if (selectId == clkId){
        m_selectId = -1;
        update();
        return false;
    }
    // 如果颜色相同，不能吃，而是换选中的棋子
    if (m_chess[selectId].m_red == m_chess[clkId].m_red){
        m_selectId = clkId;
        update();
        return false;
    }
    // 各种棋子不同的走法
    if (m_chess[selectId].m_type == Chess::JIANG){
        return canMoveJiang(selectId, clkRow, clkCol, clkId);
    } else if (m_chess[selectId].m_type == Chess::JU){
        return canMoveJu(selectId, clkRow, clkCol, clkId);
    } else if (m_chess[selectId].m_type == Chess::MA){
        return canMoveMa(selectId, clkRow, clkCol, clkId);
    } else if (m_chess[selectId].m_type == Chess::SHI){
        return canMoveShi(selectId, clkRow, clkCol, clkId);
    } else if (m_chess[selectId].m_type == Chess::XIANG){
        return canMoveXiang(selectId, clkRow, clkCol, clkId);
    } else if (m_chess[selectId].m_type == Chess::BING){
        return canMoveBing(selectId, clkRow, clkCol, clkId);
    } else if (m_chess[selectId].m_type == Chess::PAO){
        return canMovePao(selectId, clkRow, clkCol, clkId);
    } else {
        return false;
    }
}



// 鼠标点击，走子
void Board::mouseReleaseEvent(QMouseEvent* event){
    QPoint clkPos = event->pos();
    // 找到最靠近的交叉点，然后判断在不在圆内
    // TODO 可以改进的算法
    int clkRow = 0, clkCol = 0, clkId = -1;
    bool clk = getRowCol(clkPos, clkRow, clkCol);
    // 如果点到了棋盘外面，直接结束
    if (!clk) return;


    // 点到了棋子，并且棋子没死！则更新clkId
    if (m_hasChess[clkRow][clkCol] != -1 && !m_chess[m_hasChess[clkRow][clkCol]].m_dead){
        clkId = m_hasChess[clkRow][clkCol];
    }


    // 如果前面没有点到棋子，那么只是选中
    if (m_selectId == -1){
        // 这次也没点到棋子
        if (clkId == -1) return;
        // 这次点到了，且是我方回合，选中
        if (m_chess[clkId].m_red == m_redTurn){
            m_selectId = clkId;
        }
    } else {
    // 如果前面点到了棋子，则是走子或吃子
        // 判断是否符合规则
        if (!canMove(m_selectId, clkRow, clkCol, clkId)) return;

        // 走子
        // （先把点击位置的存在棋子id设置好，再把原来位置的存在棋子id设为-1）
        m_hasChess[clkRow][clkCol] = m_hasChess[m_chess[m_selectId].m_row][m_chess[m_selectId].m_col];
        m_hasChess[m_chess[m_selectId].m_row][m_chess[m_selectId].m_col] = -1;

        m_chess[m_selectId].m_row = clkRow;
        m_chess[m_selectId].m_col = clkCol;
        // 这次也点到了，吃子
        if (clkId != -1){
            m_chess[clkId].m_dead = true;
        }
        m_selectId = -1;
        m_redTurn = !m_redTurn;
    }
    update();
}












Board::~Board(){

}

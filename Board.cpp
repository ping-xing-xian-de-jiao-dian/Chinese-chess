#include "Board.h"
#include <QPainter>
#include <QRect>
#include <QMouseEvent>
#include <QVector>
#include <QDebug>
#include <math.h>


// 构造函数初始化
Board::Board(QWidget *parent) : QWidget(parent){
    // 绘制棋盘参数初始化
    m_gridSize = 80;
    m_chessR = 36;
    m_wordSize = 32;
    m_selectId = -1;
    m_redTurn = true;
    m_message = "";
    m_redTurnNum = 0;
    m_blackTurnNum = 0;
    m_redSkillPoint = 0;
    m_blackSkillPoint = 0;
    for (int i = 0; i < 10; ++i){
        for (int j = 0; j < 9; ++j){
            m_hasChess[i][j] = -1;
        }
    }

    // 32个棋子初始化
    m_redUnder = true;
    initRedBlack(m_redUnder);

    // 走棋信息
    m_messageLabel = new QLabel(this);
    m_messageLabel->setText(m_message);
    m_messageLabel->setGeometry(10 * m_gridSize, 5 * m_gridSize, 5 * m_gridSize, m_gridSize);
    m_messageLabel->setFont(QFont("楷体", m_wordSize / 2, 300));
    m_messageLabel->show();

    // 技能点信息
    m_redSkillLabel = new QLabel(this);
    m_redSkillLabel->setText("");
    m_redSkillLabel->setGeometry(10 * m_gridSize, 9 * m_gridSize, 5 * m_gridSize, m_gridSize);
    m_redSkillLabel->setFont(QFont("楷体", m_wordSize / 2, 300));
    m_redSkillLabel->show();
}


// 执红棋黑棋初始化，red为true则红棋在下面
void Board::initRedBlack(bool red){
    for (int id = 0; id < 32; ++id){
        m_chess[id].init(id, red);
        m_hasChess[m_chess[id].m_row][m_chess[id].m_col] = id;
    }
    update();
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

    // 技能点
    if (m_redUnder) {
        m_redSkillLabel->setText("我方技能点: " + QString::number(m_redSkillPoint));
        m_messageLabel->setStyleSheet("color:red;");
    } else {
        m_redSkillLabel->setText("我方技能点: " + QString::number(m_blackSkillPoint));
        m_messageLabel->setStyleSheet("color:black;");
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
    painter.setFont(QFont("楷体", m_wordSize / 2, 700));
    painter.drawText(rect, m_chess[id].getText() + m_chess[id].getLevel(), QTextOption(Qt::AlignCenter));
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


// 判断己方移动完后是否被将军，id小的是对面的
bool Board::beiJiangJun(int selectId, int clkRow, int clkCol, int clkId){
    // 我方将军id为20
    int myJiangId = 20;
    // *****为了防止对面在模仿的时候判定，例如客户端红棋将军黑棋，服务端模拟红棋时黑棋发现自己在送将，所以一定要走哪个用哪边将军判定！！！！
    if (selectId < 16) {
        myJiangId = 4;
    }
    // 移动完后，不能被对方所有棋子将军
    bool flag = false;

    // 假装走子
    int origin1 = m_hasChess[clkRow][clkCol];
    int origin2 = m_hasChess[m_chess[selectId].m_row][m_chess[selectId].m_col];
    int origin3 = m_chess[selectId].m_row;
    int origin4 = m_chess[selectId].m_col;
    bool origin5 = false; if (clkId != -1) origin5 = m_chess[clkId].m_dead;
    int origin6 = m_selectId;

    m_hasChess[clkRow][clkCol] = m_hasChess[m_chess[selectId].m_row][m_chess[selectId].m_col];
    m_hasChess[m_chess[selectId].m_row][m_chess[selectId].m_col] = -1;

    m_chess[selectId].m_row = clkRow;
    m_chess[selectId].m_col = clkCol;
    if (clkId != -1){
        m_chess[clkId].m_dead = true;
    }

    // 检查所有对方棋子能否将自己军，即canMove到自己将军位置
    int jiangJunRow = m_chess[myJiangId].m_row, jiangJunCol = m_chess[myJiangId].m_col;
    int start = 0, end = 16;
    if (selectId < 16){
        start = 16; end = 32;
    }

    for (int id = start; id < end; ++id){
        if (canMove(id, jiangJunRow, jiangJunCol, myJiangId)){
            flag = true;
            break;
        }
    }

    // 回退，取消假装走子
    m_selectId = origin6;
    if (clkId != -1) m_chess[clkId].m_dead = origin5;
    m_chess[selectId].m_col = origin4;
    m_chess[selectId].m_row = origin3;
    m_hasChess[m_chess[selectId].m_row][m_chess[selectId].m_col] = origin2;
    m_hasChess[clkRow][clkCol] = origin1;

    return flag;
}


// 将的移动规则
bool Board::canMoveJiang(int selectId, int clkRow, int clkCol, int clkId){
    int row = m_chess[selectId].m_row;
    int col = m_chess[selectId].m_col;

    // 同一列且没有阻拦，直接吃掉对面的将
    if (clkId != -1 && m_chess[clkId].m_type == Chess::JIANG){
        return canMoveJu(selectId, clkRow, clkCol, clkId);
    }

    // 范围在九宫格
    if (clkCol < 3 || clkCol > 5) return false;
    if (m_chess[selectId].m_red){
        if (!m_redUnder && (clkRow < 0 || clkRow > 2)) return false;
        else if (m_redUnder && (clkRow < 7 || clkRow > 9)) return false;
    } else {
        if (!m_redUnder && (clkRow < 7 || clkRow > 9)) return false;
        else if (m_redUnder && (clkRow < 0 || clkRow > 2)) return false;
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
        if (!m_redUnder && (clkRow < 0 || clkRow > 2)) return false;
        else if (m_redUnder && (clkRow < 7 || clkRow > 9)) return false;
    } else {
        if (!m_redUnder && (clkRow < 7 || clkRow > 9)) return false;
        else if (m_redUnder && (clkRow < 0 || clkRow > 2)) return false;
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
        if (!m_redUnder && (clkRow < 0 || clkRow > 4)) return false;
        if (m_redUnder && (clkRow < 5 || clkRow > 9)) return false;
    } else {
        if (!m_redUnder && (clkRow < 5 || clkRow > 9)) return false;
        if (m_redUnder && (clkRow < 0 || clkRow > 4)) return false;
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
        if (!m_redUnder && clkRow < 5){
            if (clkRow == row + 1 && clkCol == col) return true;
        } else if (!m_redUnder) {
            QVector<QVector<int>> dirs = {
                {1, 0}, {0, 1}, {0, -1}
            };
            for (auto dir : dirs){
                if (row + dir[0] == clkRow && col + dir[1] == clkCol){
                    return true;
                }
            }
        }
        if (m_redUnder && clkRow > 4){
            if (clkRow == row - 1 && clkCol == col) return true;
        } else if (m_redUnder) {
            QVector<QVector<int>> dirs = {
                {-1, 0}, {0, 1}, {0, -1}
            };
            for (auto dir : dirs){
                if (row + dir[0] == clkRow && col + dir[1] == clkCol){
                    return true;
                }
            }
        }
    } else {
        if (!m_redUnder && clkRow > 4){
            if (clkRow == row - 1 && clkCol == col) return true;
        } else if (!m_redUnder) {
            QVector<QVector<int>> dirs = {
                {-1, 0}, {0, 1}, {0, -1}
            };
            for (auto dir : dirs){
                if (row + dir[0] == clkRow && col + dir[1] == clkCol){
                    return true;
                }
            }
        }
        if (m_redUnder && clkRow < 5){
            if (clkRow == row + 1 && clkCol == col) return true;
        } else if (m_redUnder) {
            QVector<QVector<int>> dirs = {
                {1, 0}, {0, 1}, {0, -1}
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
    // 如果死了，那肯定是不能动的
    if (m_chess[selectId].m_dead) return false;
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


// 列号转换为中文数字（分我方还是对方）
QString Board::getChineseNum(int num, int selectId){
    if (num < 0 || num > 8) return "错误！";
    QVector<QString> nums = {
        "", "一", "二", "三", "四", "五", "六", "七", "八", "九"
    };
    // 对方
    if (selectId >= 0 && selectId < 16) return nums[1 + num];
    else if (selectId >= 16 && selectId < 32) return nums[9 - num];
    else return nums[num];
}

// 显示走子信息
void Board::chessMoveMessage(int selectId, int clkRow, int clkCol){
    int type = m_chess[selectId].m_type;
    // 判断同列上是否有相同类型的子
    int row;
    for (row = 0; row < 10; ++row){
        if (row == m_chess[selectId].m_row) continue;
        int id = m_hasChess[row][m_chess[selectId].m_col];
        if (id == -1) continue;
        // 同色，同类型，没死
        if (m_chess[id].m_red == m_chess[selectId].m_red &&
                m_chess[id].m_type == m_chess[selectId].m_type &&
                !m_chess[id].m_dead){
            break;
        }
    }
    // 文本组合
    // 前后同类型
    if (row < 10){
        if (m_chess[selectId].m_row < row){
            if (selectId >= 16) m_message = "前";
            else m_message = "后";
        } else {
            if (selectId >= 16) m_message = "后";
            else m_message = "前";
        }
        m_message += m_chess[selectId].getText();
    } else {
    // 不存在前后同类型子的情况
        m_message = m_chess[selectId].getText() + getChineseNum(m_chess[selectId].m_col, selectId);
    }
    // 后两个字，进退多少，要注意马，士，象的打谱规则不太一样
    if (clkRow != m_chess[selectId].m_row){
        if (clkRow < m_chess[selectId].m_row){
            if (selectId >= 16) m_message += "进";
            else m_message += "退";
        } else {
            if (selectId >= 16) m_message += "退";
            else m_message += "进";
        }
        if (type != Chess::MA && type != Chess::SHI && type != Chess::XIANG){
            m_message += getChineseNum(abs(m_chess[selectId].m_row - clkRow), -1);
        } else {
            m_message += getChineseNum(clkCol, selectId);
        }
    } else {
        m_message += "平";
        m_message += getChineseNum(clkCol, selectId);
    }
    m_messageLabel->setText(m_message);
    if (m_chess[selectId].m_red) m_messageLabel->setStyleSheet("color:red;");
    else m_messageLabel->setStyleSheet("color:black;");
}


// 判断是否游戏结束
bool Board::gameOver(int selectId, int clkRow, int clkCol, int clkId){
    // 判断对方是否被将军
    int start = 0, end = 16, jiangJunId = 20;
    if (selectId >= 16){
        jiangJunId = 4; start = 16; end = 32;
    }
    // 如果全都没将军，return false
    int id;
    for (id = start; id < end; ++id){
        if (canMove(id, m_chess[jiangJunId].m_row, m_chess[jiangJunId].m_col, jiangJunId)) break;
    }
    // 全都没有将军
    if (id == end) return false;


    // TODO是否绝杀，对方所有走动的情况都会beiJiangJun，按频率来
    // 算法可以改进
    if (selectId < 16){
        start = 16; end = 32; jiangJunId = 4;
    } else {
        start = 0; end = 16; jiangJunId = 20;
    }
    for (int id = start; id < end; ++id){
        for (int row = 0; row < 10; ++row){
            for (int col = 0; col < 9; ++col){
                if (canMove(id, row, col, m_hasChess[row][col]) &&
                        !beiJiangJun(id, row, col, m_hasChess[row][col])){
                    return false;
                }
            }
        }
    }
    return true;
}


// 实现点击，选中，走子，吃子等
void Board::click(int clkId, int clkRow, int clkCol){
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
        // 己方回合移动完会被将军，显示不能这么走的信息
        if (beiJiangJun(m_selectId, clkRow, clkCol, clkId)) {
            m_messageLabel->setText("不能送将！");
            if (m_chess[m_selectId].m_red) m_messageLabel->setStyleSheet("color:red;");
            else m_messageLabel->setStyleSheet("color:black;");
            return;
        }

        // 显示走子信息
        chessMoveMessage(m_selectId, clkRow, clkCol);

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

        // 绝杀判定
        if (gameOver(m_selectId, clkRow, clkCol, clkId)){
            // 游戏结束
            // 获胜方颜色
            QString winColor;
            if (m_redTurn) winColor = "红";
            else winColor = "黑";
            m_message = "绝杀，无解！\n" + winColor + "方胜利！";
            m_messageLabel->setText(m_message);
            return;
        }

        // 增加技能点
        if (m_redUnder && m_redTurn){
            ++m_redTurnNum;
            if (m_redTurnNum % 2 == 0) ++m_redSkillPoint;
        } else if (!m_redUnder && !m_redTurn) {
            ++m_blackTurnNum;
            if (m_blackTurnNum % 2 == 0) ++m_blackSkillPoint;
        }

        m_selectId = -1;
        m_redTurn = !m_redTurn;
    }
    update();
}


void Board::levelUp(int clkRow, int clkCol, int clkId){
    // 消耗一技能点
    if (m_chess[clkId].m_red){
        if (m_redSkillPoint <= 0) return;
        --m_redSkillPoint;
    } else {
        if (m_blackSkillPoint <= 0) return;
        --m_blackSkillPoint;
    }
    ++m_chess[clkId].m_level;
    update();
}


// 鼠标点击，走子
void Board::mouseReleaseEvent(QMouseEvent* event){
    m_clkPos = event->pos();

    // 找到最靠近的交叉点，然后判断在不在圆内
    int clkRow = 0, clkCol = 0, clkId = -1;
    bool clk = getRowCol(m_clkPos, clkRow, clkCol);
    // 如果点到了棋盘外面，直接结束
    if (!clk) return;

    // 左键，正常象棋
    if (event->button() == Qt::LeftButton){
        // 点到了棋子，并且棋子没死！则更新clkId
        if (m_hasChess[clkRow][clkCol] != -1 && !m_chess[m_hasChess[clkRow][clkCol]].m_dead){
            clkId = m_hasChess[clkRow][clkCol];
        }

        click(clkId, clkRow, clkCol);
    } else if (event->button() == Qt::RightButton) {
    // 右键，增加技能点
        // 点到了棋子，并且棋子没死！则升级！
        int clkId = m_hasChess[clkRow][clkCol];
        if (clkId != -1 && !m_chess[clkId].m_dead){
            levelUp(clkRow, clkCol, clkId);
        }
    }
}












Board::~Board(){

}

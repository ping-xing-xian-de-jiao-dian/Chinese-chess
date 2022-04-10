#ifndef CHESS_H
#define CHESS_H
#include <QString>

class Chess
{
public:
    Chess();
    ~Chess();

    enum TYPE {JIANG, JU, PAO, MA, BING, SHI, XIANG};

    int m_row;
    int m_col;
    int m_id;
    bool m_dead;
    bool m_red;
    TYPE m_type;

    // 根据类型返回象棋上的字
    QString getText();

    // 初始化，bool为true则红棋在下面
    void init(int, bool);
};

#endif // CHESS_H

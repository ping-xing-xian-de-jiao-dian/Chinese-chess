#include "Chess.h"

Chess::Chess(){

}




// 根据类型返回象棋上的字
QString Chess::getText(){
    if (this->m_type == JIANG){
        if (this->m_red) return "帅";
        else return "将";
    } else if (this->m_type == SHI){
        if (this->m_red) return "仕";
        else return "士";
    } else if (this->m_type == XIANG){
        if (this->m_red) return "相";
        else return "象";
    } else if (this->m_type == BING){
        if (this->m_red) return "兵";
        else return "卒";
    }else if (this->m_type == JU){
        return "車";
    } else if (this->m_type == MA){
        return "馬";
    } else if (this->m_type == PAO){
        return "炮";
    } else {
        return "错误";
    }
}



// 初始化
void Chess::init(int id){
    m_id = id;
    m_dead = false;
    m_red = (id < 16);
    // 位置确定
    struct {
        int row, col;
        Chess::TYPE type;
    } pos[16] = {
        {0,0,Chess::JU},
        {0,1,Chess::MA},
        {0,2,Chess::XIANG},
        {0,3,Chess::SHI},
        {0,4,Chess::JIANG},
        {0,5,Chess::SHI},
        {0,6,Chess::XIANG},
        {0,7,Chess::MA},
        {0,8,Chess::JU},
        {2,1,Chess::PAO},
        {2,7,Chess::PAO},
        {3,0,Chess::BING},
        {3,2,Chess::BING},
        {3,4,Chess::BING},
        {3,6,Chess::BING},
        {3,8,Chess::BING}
    };

    if (m_red){
        m_row = pos[id].row;
        m_col = pos[id].col;
        m_type = pos[id].type;
    } else {
        m_row = 9 - pos[id - 16].row;
        m_col = 8 - pos[id - 16].col;
        m_type = pos[id - 16].type;
    }
}



Chess::~Chess(){

}

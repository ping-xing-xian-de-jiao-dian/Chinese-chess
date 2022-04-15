#include "NetGame.h"
#include <QDebug>

const QString ip = "192.168.3.38";
const int port = 9999;


/* 给对方发送的消息
 * 1. 执红方还是黑方，服务器发出，客户端接收
 *    [1, 1/0]第一个字节固定1，第二个字节1表示接收方走红棋，0表示接收方走黑棋
 * 2. 点击信息，点击的坐标，棋子Id，所有棋子等级, 走子信息
 *    [2, row, col, clkId, clkLevel0~clkLevel31, 100]
 * 3. 右键升级，点击的坐标，棋子Id
 *    [3, row, col, clkId]
 */


NetGame::NetGame(bool isServer) {
    // 随机数，用于随机接收方是红方还是黑方
    srand(unsigned(time(nullptr)));

    m_server = nullptr;
    m_socket = nullptr;
    m_connectLabel = nullptr;

    if (isServer){
        m_server = new QTcpServer(this);
        // 监听
        m_server->listen(QHostAddress::Any, port);
        // 连接
        // 当有人来连接我们的时候，slotNewConnection()函数被调用
        connect(m_server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    } else {
        m_socket = new QTcpSocket(this);
        m_socket->connectToHost(QHostAddress(ip), port);
        // 连接后调用接收函数slotRecv()
        connect(m_socket, SIGNAL(readyRead()), this, SLOT(slotRecv()));
    }
}


// 显示连接信息
void NetGame::showConnectMessage(){
    if (m_connectLabel) return;
    m_connectLabel = new QLabel(this);
    m_connectLabel->setText("connected");
    m_connectLabel->setGeometry(10 * m_gridSize, m_gridSize, 5 * m_gridSize, m_gridSize);
    m_connectLabel->setFont(QFont("楷体", m_wordSize / 2, 300));
    m_connectLabel->show();
}


// 鼠标点击，选中，走子，吃子并发送
void NetGame::click(int clkId, int clkRow, int clkCol){
    // 网络游戏中，只有轮到自己时才能选中进行操作
    if (m_selectId == -1 && clkId != -1){
        // 棋子颜色要和操作人所在边的颜色相同
        if (m_redUnder != m_chess[clkId].m_red){
            return;
        }
    }

    Board::click(clkId, clkRow, clkCol);

    char buff[136];
    buff[0] = 2;
    buff[1] = char(9 - clkRow);
    buff[2] = char(8 - clkCol);
    // 棋子Id转换
    if (clkId < 16 && clkId > -1) clkId += 16;
    else if (clkId >= 16) clkId -= 16;
    buff[3] = char(clkId);

    for (int id = 0; id < 32; ++id){
        int sid = id;
        if (sid < 16) sid +=16;
        else sid -= 16;
        buff[id + 4] = char(m_chess[sid].m_level);
    }

    m_socket->write(buff, 136);
}


// 鼠标右键升级信息
void NetGame::levelUp(int selectId, int clkRow, int clkCol, int clkId){
    Board::levelUp(selectId, clkRow, clkCol, clkId);

    char buff[5];
    buff[0] = 3;

    // 棋子Id转换
    if (selectId < 16 && selectId > -1) selectId += 16;
    else if (selectId >= 16) selectId -= 16;

    buff[1] = char(selectId);
    buff[2] = char(9 - clkRow);
    buff[3] = char(8 - clkCol);

    // 棋子Id转换
    if (clkId < 16 && clkId > -1) clkId += 16;
    else if (clkId >= 16) clkId -= 16;

    buff[4] = char(clkId);
    m_socket->write(buff, 5);
}


// 客户端连接后调用函数
void NetGame::slotNewConnection(){
    showConnectMessage();
    // 如果socket不为空，不接受其它连接
    if (m_socket) return;

    // 用来通信
    m_socket = m_server->nextPendingConnection();
    // 连接后调用接收函数slotRecv()
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(slotRecv()));

    // 给对方发送数据
    // 执红方还是黑方，服务器发出，客户端接收1为红
    char buff[2];
    buff[0] = 1;
    int ranNum = rand() % 2;
    buff[1] = char(ranNum);
    // 发送2个字节
    m_socket->write(buff, 2);

    // 棋盘翻转
    if (buff[1] == 1) m_redUnder = true;
    else m_redUnder = false;
    initRedBlack(m_redUnder);
}


// 服务端连接后调用函数
void NetGame::slotRecv(){
    showConnectMessage();
    // 接收到的消息，字节数组
    QByteArray buff = m_socket->readAll();
    char command = buff[0];
    if (command == 1){
        // 执红执黑
        char data = buff[1];

        // 棋盘翻转
        if (data == 0) m_redUnder = true;
        else m_redUnder = false;
        initRedBlack(m_redUnder);
    } else if (command == 2) {
        int row = buff[1];
        int col = buff[2];
        int id = buff[3];
        for (int id = 0; id < 32; ++id) {
            m_chess[id].m_level = buff[id + 4];
        }

        // 如果是选中棋子，不给对方看
        // 如果更换选子，也不给对方看
        if ((m_selectId == -1 && m_chess[id].m_red == m_redTurn) ||
                (m_selectId != -1 && id != -1 && m_chess[m_selectId].m_red == m_chess[id].m_red)){
            m_selectId = id; return;
        }
        // 走子，吃子，无脑复制
        Board::click(id, row, col);
    } else if (command == 3) {
    // 右键传递
        int selectId = buff[1];
        int row = buff[2];
        int col = buff[3];
        int clkId = buff[4];
        Board::levelUp(selectId, row, col, clkId);
    }
}



NetGame::~NetGame(){
    m_socket->close();
    m_server->close();
}

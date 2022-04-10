#ifndef NETGAME_H
#define NETGAME_H
#include "Board.h"
#include <QTcpServer>
#include <QTcpSocket>

/* 给对方发送的消息
 * 1. 执红方还是黑方，服务器发出，客户端接收
 *    [1, 1/0]第一个字节固定1，第二个字节1表示接收方走红棋，0表示接收方走黑棋
 * 2. 点击信息，点击的坐标，棋子Id
 *    [2, row, col, clkId]
 */


// 无服务器的（服务器客户端对等）
class NetGame : public Board {
    Q_OBJECT
public:
    NetGame(bool isServer);
    ~NetGame();

    // 网络服务，服务端两个都用，客户端只用socket
    QTcpServer* m_server;
    QTcpSocket* m_socket;


    // 鼠标点击，选中，走子，吃子并发送
    void click(int, int, int);


// 槽函数
public slots:
    void slotNewConnection();
    void slotRecv();
};

#endif // NETGAME_H

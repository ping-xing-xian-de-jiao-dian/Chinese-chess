#ifndef NETGAME_H
#define NETGAME_H
#include "Board.h"
#include <QTcpServer>
#include <QTcpSocket>


// 无服务器的（服务器客户端对等）
class NetGame : public Board {
    Q_OBJECT
public:
    NetGame(bool isServer);
    ~NetGame();

    // 网络服务，服务端两个都用，客户端只用socket
    QTcpServer* m_server;
    QTcpSocket* m_socket;
    QLabel* m_connectLabel;


    // 鼠标点击，选中，走子，吃子并发送
    void click(int, int, int);

    // 显示连接信息
    void showConnectMessage();

    // 升级信息
    void levelUp(int, int, int);


// 槽函数
public slots:
    void slotNewConnection();
    void slotRecv();
};

#endif // NETGAME_H

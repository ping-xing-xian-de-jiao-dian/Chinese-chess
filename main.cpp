#include "mainwindow.h"
#include <QApplication>
#include "Board.h"
#include "NetGame.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    // QApplication抽象了QT的应用程序
    QApplication a(argc, argv);

    // 询问是否作为服务器启动
    QMessageBox::StandardButton res;
    res = QMessageBox::question(nullptr, "server or client", "作为服务器启动");
    bool isServer = false;
    if (res == QMessageBox::Yes){
        isServer = true;
    }

    // 抽象了窗口
    NetGame board(isServer);
    board.resize(800,900);
    board.show();

    // 进入消息循环，等待用户输入（点击、拖动等）
    return a.exec();
}

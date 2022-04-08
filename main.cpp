#include "mainwindow.h"
#include <QApplication>
#include "Board.h"
#include "NetGame.h"

int main(int argc, char *argv[])
{
    // QApplication抽象了QT的应用程序
    QApplication a(argc, argv);
    // 抽象了窗口
    NetGame board;
    board.resize(800,900);
    board.show();

    // 进入消息循环，等待用户输入（点击、拖动等）
    return a.exec();
}

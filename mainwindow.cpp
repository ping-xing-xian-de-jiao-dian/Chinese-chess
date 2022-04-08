#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>


// 构造函数初始化
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    m_button = new QPushButton("悔棋", this);
}


// 画棋盘
void MainWindow::paintEvent(QPaintEvent *){
    // 绘制类，参数是绘制对象
    QPainter painter(this);

    if (m_pt_click != QPoint(0, 0)){
        painter.drawEllipse(m_pt_click, 50, 50);
    }
}


// 鼠标点击事件
void MainWindow::mousePressEvent(QMouseEvent *event){
    // 得到鼠标点击的地方，并且更新（重新绘制界面）
    m_pt_click = event->pos();
    update();
}



MainWindow::~MainWindow()
{
    delete ui;
}

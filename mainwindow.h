#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    // 类中斜体字表示虚函数
    void paintEvent(QPaintEvent* );

    // 鼠标点击事件
    void mousePressEvent(QMouseEvent *);

    // 按钮
    QPushButton* m_button;
    // 鼠标点击位置
    QPoint m_pt_click;

    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

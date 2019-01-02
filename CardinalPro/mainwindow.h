#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QPainter>
#include<QMouseEvent>
#include<cspline.h>
#include<QLabel>
#include <QPropertyAnimation>
#include<QTime>
#include<QTimer>
#include <QPixmap>
#include<QPixmapCache>
#include<QDebug>
#include<QMovie>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
     Q_OBJECT
public:
    double px[100];
    double py[100];
    int flag;
    int n;
    int grain;
    double tension;
    double speed;
    int count;
    bool isParameterized;//是否弧长参数化

    QVector<QPoint> mPoints;//从画布上获取的点的坐标
    CSpline *mCSpline;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void paintEvent(QPaintEvent *ev);
    void mousePressEvent(QMouseEvent *ev);

private slots:
    void on_Draw_pushButton_clicked();//点击绘制按钮
    void on_Clear_pushButton_clicked();//点击清除画布按钮
    void on_Points_pushButton_clicked();//点击显示插值点按钮
    void on_Play_pushButton_clicked();//点击播放按钮
    void timer1Rotate();//普通模式下的计时器
    void timer2Rotate();//弧长参数化模式的计时器

private:
    QImage* img;
    QMovie* mov;
    QTimer * timer1;
    QTimer * timer2;
    QLabel *obj;
    QPropertyAnimation  *ani;
    QPropertyAnimation  *ani2;
    int ticks;
    bool isImage;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

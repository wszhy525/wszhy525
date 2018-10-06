#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cv.h"
#include "highgui.h"
#include <QTime>
#include <QKeyEvent>
#include<QLineEdit>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QLineEdit *edit = new QLineEdit(this);

private slots:

    void cameraopen();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();


    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
        //QTimer 计时用；

        IplImage* frame;
        //申请IplImage类型指针，就是申请内存空间来存放每一帧图像

        CvCapture *cam;
        // 视频获取结构， 用来作为视频获取函数的一个参数
        char filename[200];

};


#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QKeyEvent>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
      //初始化timer

      cam = NULL;
      // 初始化cam

      connect(timer, SIGNAL(timeout()), this, SLOT(cameraopen()));
      ui->pushButton_3->setShortcut(Qt::Key_S);

}

MainWindow::~MainWindow()
{
    delete ui;
    cvReleaseCapture(&cam);
}
void MainWindow::cameraopen()
{
    frame = cvQueryFrame(cam);
    // 从摄像头中抓取并返回每一帧

    QImage image = QImage((const uchar*)frame->imageData, frame->width, frame->height, QImage::Format_RGB888).rgbSwapped();
    // 将抓取到的帧，转换为QImage格式。QImage::Format_RGB888不同的摄像头用不同的格式。


    QGraphicsScene *scene = new QGraphicsScene;
    //创建图片显示方式的容器

    scene->addPixmap(QPixmap::fromImage(image));
    //装载图片

    ui->graphicsView->setScene(scene);

    //显示图片，自动添加滚动条

    ui->graphicsView->show();
    //显示图片


}

void MainWindow::on_pushButton_clicked()
{
    cam = cvCreateCameraCapture(0);
       //打开摄像头，从摄像头中获取视频

       timer->start(33);
       // 开始计时，超时则发出timeout()信号
}

void MainWindow::on_pushButton_2_clicked()
{
    timer->stop();
      // 停止读取数据。

      cvReleaseCapture(&cam);
      //释放内存；

}


void MainWindow::on_pushButton_3_clicked()

{
    frame = cvQueryFrame(cam);
    QTime current_time =QTime::currentTime();
    int hour = current_time.hour();//当前的小时
    int minute = current_time.minute();//当前的分
    int second = current_time.second();//当前的秒
    int msec = current_time.msec();//当前的毫秒
    sprintf(filename, "%s%d%d%d%d%s","save",hour,minute,second,msec,".jpg");
    cvSaveImage(filename,frame);
}

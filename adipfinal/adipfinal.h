#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <QtWidgets/QMainWindow>
#include "ui_adipfinal.h"
#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <stdio.h>
#include <iostream>
#include <QFileDialog>
#include <QLabel>
#include<QDialog>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <conio.h>
#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <qDebug>
#include <opencv2/core/mat.hpp>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <stack>

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <stack>
#include <QProgressBar>
using namespace cv;
using namespace std;
//class QLineEdit;
class adipfinal : public QMainWindow
{
    Q_OBJECT

public:
    //adipfinal(QWidget* parent);
    
    adipfinal(QWidget *parent = Q_NULLPTR); 
    //Mat src;
    QLabel(parent);
    //QLabel timelabel;
    QProgressBar progressBar;
    int globalMark_import = 0;//0沒圖1有圖
    Scalar color =Scalar(255, 0, 0);;//bgr
    int redcolorset = 0;
    int greencolorset = 0;
    int blackcolorset=0,yellowcolorset=0;
    QImage image;
    QImage image2;
    QImage globalImg_temp;
    Mat mat_import;
    Mat mat_import2;
    Mat img_back_one;
    Mat src;
    Mat inputslic;
    Mat slic_black;
    int globalX =0;
    int globalY =0;
    int globalX_release=0;
    int globalY_release = 0;
    int slicobjX_release = 0;
    int slicobjY_release = 0;
    int globalXY_isPress = 0;//沒按
    int globalY_move = 8;
    int globalX_move = -8;
    int objectX_move = 0,objectY_move = 0;
    int backgroundX_move = 0, backgroundY_move = 0;
    int temp_x = 0;
    int temp_y = 0;
    int frame=1;
    double speed = 10;
    int superscaler = 0;//未按狀態為0
    int objupdatenum = 0;
    int save = 0;//0存morph   1存SLIC
    double time = 0;
    QString fileName;
    stack<int> slicobjseedStack;    // 堆疊
    stack<int> seedStack;    // 堆疊
    stack<int> backgroundStack;    // 堆疊
    stack<int> backgroundmoveStack;    // 堆疊
    int w, h;
    int execution_num = 0;
    LARGE_INTEGER m_nFreq;
    LARGE_INTEGER m_nBeginTime;
    LARGE_INTEGER nEndTime;
private:
    Ui::adipfinalClass ui;
    //QLineEdit* speedLineEdit;
private slots:
    
    
    
    
    void on_importButton_clicked();
    void on_saveButton_clicked();
    void on_executionButton_clicked();
    void on_object_slicButton_clicked();
    void on_clearButton_clicked();
    void on_redcheckBox_clicked(bool checked);
    void on_greencheckBox_clicked(bool checked);
    void on_blackcheckBox_clicked(bool checked);
    void on_yellowcheckBox_clicked(bool checked);
    bool eventFilter(QObject* obj, QEvent* event);//事件偵測(滑鼠)
};

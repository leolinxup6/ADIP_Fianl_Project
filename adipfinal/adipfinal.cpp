#include "adipfinal.h"
#include "function.h"

using namespace cv;


adipfinal::adipfinal(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    qApp->installEventFilter(this);
    
    //QString str = speedLineEdit->text();
    //speed = str.toDouble();
    
    
    QueryPerformanceFrequency(&m_nFreq); // 獲取時鍾周期
    QueryPerformanceCounter(&m_nBeginTime); // 計時開始
    QueryPerformanceCounter(&nEndTime);// 計時截止
    time = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;
    QLabel* timelabel = new QLabel(this);
    ui.timelabel->setText(QString(" %1").arg(time));
    update();
}


QImage cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if (mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for (int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar* pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++)
        {
            uchar* pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if (mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar* pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if (mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar* pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}
cv::Mat QImage2cvMat(QImage image)
{
    cv::Mat mat;
    qDebug() << image.format();
    switch (image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, COLOR_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

void adipfinal::on_importButton_clicked()
{
    while(!seedStack.empty()) seedStack.pop();
    while (!backgroundStack.empty()) backgroundStack.pop();
    while (!backgroundmoveStack.empty()) backgroundmoveStack.pop();
    while (!slicobjseedStack.empty()) slicobjseedStack.pop();
    ui.greencheckBox->setChecked(false);
    greencolorset = 0;
    ui.redcheckBox->setChecked(false);
    redcolorset = 0;
    ui.yellowcheckBox->setChecked(false);
    yellowcolorset = 0;
    ui.blackcheckBox->setChecked(false);
    blackcolorset = 0;
    globalMark_import = 0;//0沒圖1有圖    
    redcolorset = 0;
    greencolorset = 0;
    blackcolorset = 0, yellowcolorset = 0;
    execution_num = 0;
    globalX = 0;
    globalY = 0;
    globalX_release = 0;
    globalY_release = 0;
    slicobjX_release = 0;
    slicobjY_release = 0;
    globalXY_isPress = 0;//沒按
    globalY_move = 8;
    globalX_move = -8;
    objectX_move = 0, objectY_move = 0;
    backgroundX_move = 0, backgroundY_move = 0;
    temp_x = 0;
    temp_y = 0;
    frame = 1;
    speed = 10;
    superscaler = 0;//未按狀態為0
    objupdatenum = 0;
    save = 0;//0存morph   1存SLIC
    time = 0;
    
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Image files(*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm );;All files (*.*)"));
    dialog.setViewMode(QFileDialog::Detail);
    fileName = QFileDialog::getOpenFileName(this, tr("open image file"), "./", tr("Image files(*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm );;All files (*.*)"));

    String c_fileName = fileName.toStdString();    
    src = imread(c_fileName, IMREAD_COLOR);
    
    QueryPerformanceFrequency(&m_nFreq); // 獲取時鍾周期
    QueryPerformanceCounter(&m_nBeginTime); // 計時開始
    //imshow("test", src);
    if (!fileName.isEmpty())
    {
        QImage image(fileName);       
        //w = ui.imgshowLabel->width();
        //h = ui.imgshowLabel->height();
        w = src.rows;
        h = src.cols;
        ui.imgshowLabel -> setPixmap(QPixmap::fromImage(image));
        globalMark_import = 1;
        globalImg_temp = image;     
        ui.imgshowLabel->resize(h, w);
        if (w > 400) {
            resize(h + 200, w + 200);
        }
        
        
        
       
        /*image = image.convertToFormat(QImage::Format_RGB888);
        mat_import = QImage2cvMat(image);
        imshow("QImage2cvMat RGB24", mat_import);
        QImage image2 = cvMat2QImage(mat_import);
        ui.imgshowLabel->setPixmap(QPixmap::fromImage(image2).scaled(w, h, Qt::KeepAspectRatio));
        QMessageBox::information(this, "message", "success2");
        */
        
        QueryPerformanceCounter(&nEndTime);// 計時截止
        time = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;
        QLabel* timelabel = new QLabel(this);
        ui.timelabel->setText(QString(" %1 ms").arg(time));
        update();
        QMessageBox::information(this, "message", "success");
        return;
    }
    else {
        QMessageBox::information(this, "message", "input error");
        globalMark_import = 0;//沒圖
    }
    
}

void adipfinal::on_saveButton_clicked()
{
    QString str = ui.lineEdit->text();
    speed = str.toDouble();
    if (str == NULL) { speed = 10; }
    qDebug() << speed << "speed";

    frame = speed;
    Mat input;
    input = src.clone();
    //Point seed = Point(globalX_release, globalY_release);


    //Rect ROI_rect;
    //Mat object = Trace(input, seed, ROI_rect);
    //Mat background = Background(input, object);
    //Mat ROI = input(ROI_rect);
    //Mat mask = object(ROI_rect);
    //movestep move;
    //int i = 0;
    
    
    char c = (char)waitKey(25);
    if (globalMark_import == 1)//r如果標記為1，證明有檔案載入，不然沒有不需要儲存
    {
        if (save == 0) {
            QueryPerformanceFrequency(&m_nFreq); // 獲取時鍾周期
            QueryPerformanceCounter(&m_nBeginTime); // 計時開始
            Savevideo2(input, seedStack, backgroundStack, backgroundmoveStack, objectX_move, objectY_move, frame, c);
            
            QueryPerformanceCounter(&nEndTime);// 計時截止
            time = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;
            QLabel* timelabel = new QLabel(this);
            
            ui.timelabel->setText(QString(" %1 ms").arg(time));
            QMessageBox::information(this, "message", "success save Origin");
            update();
        }
        else{
            QueryPerformanceFrequency(&m_nFreq); // 獲取時鍾周期
            QueryPerformanceCounter(&m_nBeginTime); // 計時開始
            Savevideo3(input, slicobjseedStack, backgroundStack, backgroundmoveStack, objectX_move, objectY_move, frame, c);//SLIC
            QueryPerformanceCounter(&nEndTime);// 計時截止
            time = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;
            QLabel* timelabel = new QLabel(this);
            ui.timelabel->setText(QString(" %1 ms").arg(time));
            update();
            QMessageBox::information(this, "message", "success save SLIC");

        }
    }
    else
    {
        QMessageBox::information(this, "message", "error2");
    }
}
    
void adipfinal::on_executionButton_clicked()
{
    //cv::VideoCapture cap("123.gif");
    //cv::VideoCapture cap("street.avi");
    /*if (!cap.isOpened()) {
        QMessageBox::information(this, "message", "error3");
    }
    int i = 0;
    while (1) {

        cv::Mat frame;

        cap >> frame;
        if (frame.empty()) { return on_executionButton_clicked(); }

        imshow("Frame", frame);
        char c = (char)waitKey(25);
        i = getch();
        if (c == 27||i==27)
            break;
    }

    cap.release();
    destroyAllWindows();*/
    //QMessageBox::information(this, "message", QString("x:%1 y:%2 x1:%3 y1:%4").arg(globalX).arg(globalY).arg(globalX_release).arg(globalY_release));
    //ui.imgshowLabel->setPixmap(QPixmap::fromImage(globalImg_temp).scaled(w, h, Qt::KeepAspectRatio));
    //QMessageBox::information(this, "message", "success");



        //ui.lineEdit->placeholderText();
    QString str = ui.lineEdit->text();
    speed = str.toDouble();
    if (str == NULL) { speed = 10; }
    qDebug() << speed << "speed";

    frame = speed;
    Mat input;
    input = src.clone();
    save = 0;
    //Point seed = Point(globalX_release, globalY_release);
    QString savefile = ui.savenamelineEdit->text();
    QByteArray byteArray = savefile.toLocal8Bit();
    char* char_savefile = byteArray.data();
    if (savefile == NULL) { char_savefile = "test.avi"; }
    //Rect ROI_rect;
    //Mat object = Trace(input, seed, ROI_rect);
    //Mat background = Background(input, object);
    //Mat ROI = input(ROI_rect);
    //Mat mask = object(ROI_rect);
    //movestep move;
    //int i = 0;
    char c = (char)waitKey(25);
    //i = getch();
    movestep dynamic_rate;
    dynamic_rate.x = 0;
    dynamic_rate.y = -10;
    execution_num = 0;
    while (execution_num == 0) {
        //Videoplayer(input, seedStack, objectX_move, objectY_move, frame,c);
        QueryPerformanceFrequency(&m_nFreq); // 獲取時鍾周期
        QueryPerformanceCounter(&m_nBeginTime); // 計時開始
        Videoplayer2(input, seedStack, backgroundStack, backgroundmoveStack, objectX_move, objectY_move, frame, char_savefile,c);//SLIC
        QueryPerformanceCounter(&nEndTime);// 計時截止
        time = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;
        QLabel* timelabel = new QLabel(this);
        ui.timelabel->setText(QString(" %1 ms").arg(time));

        update();
        execution_num = 1;

    }
    while (execution_num == 1) {


        VideoCapture cap(char_savefile);
        if (!cap.isOpened())
            break;
        while (1) {
            Mat frame;
            cap >> frame;
            if (frame.empty())
                break;
            imshow("output", frame);
            waitKey(1000);
            c = (char)waitKey(25);
            if (c == 27) {
                execution_num = 2;
                break;
            }
        }
       
        cap.release();
        //destroyAllWindows();

    }
    destroyAllWindows();
    execution_num = 0;
}

    
   
void adipfinal::on_object_slicButton_clicked()
{
    save = 1;
    Point a;
    Rect newROI_rect;
    QString savefile = ui.savenamelineEdit->text();
    QByteArray byteArray = savefile.toLocal8Bit();
    char* char_savefile = byteArray.data();
    if (savefile == NULL) { char_savefile = "test.avi"; }
    QString str = ui.lineEdit->text();
    speed = str.toDouble();
    if (str == NULL) { speed = 10; }
    

    if (superscaler == 0) {
        Mat input;
        
        input = src.clone();
        //input = SLIC_trace(input, slicobjseedStack, newROI_rect, superscaler);
        inputslic = SLIC_trace1(input, inputslic, slic_black, slicobjseedStack, newROI_rect, superscaler);
        image2 = cvMat2QImage(inputslic);
        ui.imgshowLabel->setPixmap(QPixmap::fromImage(image2));
        superscaler = 1;
        /// <summary>
        /// //////////////////////////////
        /// </summary>
        /*globalImg_temp = globalImg_temp.convertToFormat(QImage::Format_RGB888);
        cv::Mat mat = QImage2cvMat(globalImg_temp);*/

        /*image2 = cvMat2QImage(input);
        ui.imgshowLabel->setPixmap(QPixmap::fromImage(image2));
        superscaler = 1;*/

    }
    else if (superscaler == 1) {
        Mat input;
        input = src.clone();
        //input = SLIC_trace(input, slicobjseedStack, newROI_rect, superscaler);
        inputslic = SLIC_trace1(input, inputslic,slic_black,slicobjseedStack, newROI_rect, superscaler);
        image2 = cvMat2QImage(inputslic);
        ui.imgshowLabel->setPixmap(QPixmap::fromImage(image2));
        superscaler = 2;
        /// <summary>
        /// //////////////////////////////
        /// </summary>
        /*globalImg_temp = globalImg_temp.convertToFormat(QImage::Format_RGB888);
        cv::Mat mat = QImage2cvMat(globalImg_temp);*/

        /*image2 = cvMat2QImage(input);
        ui.imgshowLabel->setPixmap(QPixmap::fromImage(image2));
        superscaler = 2;*/
    }
    else if (superscaler == 2) {

        /*SLIC_trace(input, slicobjseedStack, newROI_rect, superscaler);*/
        ui.imgshowLabel->setPixmap(QPixmap::fromImage(globalImg_temp));
        QString str = ui.lineEdit->text();
        speed = str.toDouble();
        if (str == NULL) { speed = 10; }
        qDebug() << speed << "speed";
        
        frame = speed;



        char c = (char)waitKey(25);
        Mat input;
        input = src.clone();
        movestep dynamic_rate;
        dynamic_rate.x = 0;
        dynamic_rate.y = -10;
        execution_num = 0;
        while (execution_num == 0) {
            //Videoplayer(input, seedStack, objectX_move, objectY_move, frame,c);
            QueryPerformanceFrequency(&m_nFreq); // 獲取時鍾周期
            QueryPerformanceCounter(&m_nBeginTime); // 計時開始
            
            Videoplayer3(input, inputslic, slic_black, slicobjseedStack,backgroundStack, backgroundmoveStack, objectX_move, objectY_move, frame, char_savefile,c);//SLIC
            QueryPerformanceCounter(&nEndTime);// 計時截止
            time = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;
            QLabel* timelabel = new QLabel(this);
            ui.timelabel->setText(QString(" %1 ms").arg(time));

            update();
            execution_num = 1;

        }
        while (execution_num == 1) {


            VideoCapture cap(char_savefile);
            if (!cap.isOpened())
                break;
            while (1) {
                Mat frame;
                cap >> frame;
                if (frame.empty())
                    break;
                imshow("output", frame);
                waitKey(1000);
                c = (char)waitKey(25);
                if (c == 27)
                {
                    execution_num = 2;
                    break;
                }
            }
           /* c = (char)waitKey(25);
            if (c == 27)
            {
                execution_num = 2;
                break;
            }*/
            cap.release();
            //destroyAllWindows();

        }
        destroyAllWindows();
        
        //destroyAllWindows();
        /*image2.convertToFormat(QImage::Format_RGB888);
        cv::Mat mat2 = QImage2cvMat(image2);*/

        //cv::cvtColor(mat, mat, CV_BGR2RGB);
        //imshow("QImage2cvMat RGB32", mat);
        //image = cvMat2QImage(mat_import);
        //ui.imgshowLabel->setPixmap(QPixmap::fromImage(globalImg_temp));
        //return ;
    }
}
    

void adipfinal::on_clearButton_clicked()
{
    while (!seedStack.empty()) seedStack.pop();
    while (!backgroundStack.empty()) backgroundStack.pop();
    while (!backgroundmoveStack.empty()) backgroundmoveStack.pop();
    while (!slicobjseedStack.empty()) slicobjseedStack.pop();
    ui.greencheckBox->setChecked(false);
    greencolorset = 0;
    ui.redcheckBox->setChecked(false);
    redcolorset = 0;
    ui.yellowcheckBox->setChecked(false);
    yellowcolorset = 0;
    ui.blackcheckBox->setChecked(false);
    blackcolorset = 0;
    globalMark_import = 0;//0沒圖1有圖    
    redcolorset = 0;
    greencolorset = 0;
    blackcolorset = 0, yellowcolorset = 0;
    globalX = 0;
    globalY = 0;
    globalX_release = 0;
    globalY_release = 0;
    slicobjX_release = 0;
    slicobjY_release = 0;
    globalXY_isPress = 0;//沒按
    globalY_move = 8;
    globalX_move = -8;
    objectX_move = 0, objectY_move = 0;
    backgroundX_move = 0, backgroundY_move = 0;
    temp_x = 0;
    temp_y = 0;
    frame = 1;
    speed = 10;
    superscaler = 0;//未按狀態為0
    objupdatenum = 0;
    save = 0;//0存morph   1存SLIC
    time = 0;

    QFileDialog dialog(this);
    //dialog.setNameFilter(tr("Image files(*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm );;All files (*.*)"));
    //dialog.setViewMode(QFileDialog::Detail);
    //fileName = QFileDialog::getOpenFileName(this, tr("open image file"), "./", tr("Image files(*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm );;All files (*.*)"));

    String c_fileName = fileName.toStdString();
    src = imread(c_fileName, IMREAD_COLOR);
    w = src.rows;
    h = src.cols;
    QImage image(fileName);
    ui.imgshowLabel->setPixmap(QPixmap::fromImage(image));
    globalMark_import = 1;
    globalImg_temp = image;
    ui.imgshowLabel->resize(h, w);
    if (w > 300) {
        resize(h + 200, w + 200);
    }
}
void adipfinal::on_redcheckBox_clicked(bool checked)
{
    if (checked)
    {
        color = Scalar(0, 0, 255);
        redcolorset = 1;
        ui.greencheckBox->setChecked(false);
        greencolorset = 0;
        ui.yellowcheckBox->setChecked(false);
        yellowcolorset = 0;
        ui.blackcheckBox->setChecked(false);
        blackcolorset = 0;
        
        
        //QMessageBox::information(this, "message", QString("x:%1 y:%2 x1:%3 y1:%4").arg(globalX).arg(globalY).arg(globalX_release).arg(globalY_release));
    }    
    else {
        QMessageBox::information(this, "message", "only check one red else");
        color = Scalar(255, 0, 0);
        redcolorset = 0;
    }
    
}
void adipfinal::on_greencheckBox_clicked(bool checked)
{

    if (checked)
    {
        color = Scalar(0, 255, 0);
        greencolorset = 1;       
        ui.redcheckBox->setChecked(false);
        redcolorset = 0;
        ui.yellowcheckBox->setChecked(false);
        yellowcolorset = 0;
        ui.blackcheckBox->setChecked(false);
        blackcolorset = 0;
        //QMessageBox::information(this, "message", QString("x:%1 y:%2 x1:%3 y1:%4").arg(globalX).arg(globalY).arg(globalX_release).arg(globalY_release));
    }    
    else {
        QMessageBox::information(this, "message", "only check one greenelse");
        color = Scalar(255, 0, 0);
        greencolorset = 0;
    }

}
void adipfinal::on_blackcheckBox_clicked(bool checked)
{

    if (checked )
    {
        color = Scalar(0, 0, 0);
        blackcolorset = 1;
        ui.redcheckBox->setChecked(false);
        redcolorset = 0;
        ui.yellowcheckBox->setChecked(false);
        yellowcolorset = 0;
        ui.greencheckBox->setChecked(false);
        greencolorset = 0;
        //QMessageBox::information(this, "message", QString("x:%1 y:%2 x1:%3 y1:%4").arg(globalX).arg(globalY).arg(globalX_release).arg(globalY_release));
    }
    else {
        QMessageBox::information(this, "message", "only check one greenelse");
        color = Scalar(255, 0, 0);
        blackcolorset = 0;
    }

}
void adipfinal::on_yellowcheckBox_clicked(bool checked)
{

    if (checked)
    {
        color = Scalar(0, 255, 255);
        yellowcolorset = 1;
        ui.redcheckBox->setChecked(false);
        redcolorset = 0;
        ui.blackcheckBox->setChecked(false);
        blackcolorset = 0;
        ui.greencheckBox->setChecked(false);
        greencolorset = 0;
        //QMessageBox::information(this, "message", QString("x:%1 y:%2 x1:%3 y1:%4").arg(globalX).arg(globalY).arg(globalX_release).arg(globalY_release));
    }
    else {
        QMessageBox::information(this, "message", "only check one greenelse");
        color = Scalar(255, 0, 0);
        yellowcolorset = 0;
    }


}
bool adipfinal::eventFilter(QObject* obj, QEvent* event)
{
    QString str;
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    QPoint pt = mouseEvent->pos();
    globalImg_temp.convertToFormat(QImage::Format_RGB888);
    mat_import = QImage2cvMat(globalImg_temp);
    image2.convertToFormat(QImage::Format_RGB888);
    mat_import2 = QImage2cvMat(image2);
    if (event->type() == QEvent::MouseMove)
    {
        mouseEvent = static_cast<QMouseEvent*>(event);
        str = "MouseMove" + QString::number(pt.x()) + ", " + QString::number(pt.y());
        setWindowTitle(str);                            // 顯示座標
        

    }
    if (event->type() == QEvent::MouseButtonPress && obj->objectName() == "imgshowLabel" && globalXY_isPress == 0) {
        str = "MouseButtonPress" + QString::number(pt.x()) + ", " + QString::number(pt.y());
        setWindowTitle(str);                            // 顯示座標

        mat_import.copyTo(img_back_one);
        globalX = pt.x();
        globalY = pt.y();

        globalXY_isPress = 1;

    }
    if (superscaler == 0) {
        if (event->type() == QEvent::MouseButtonRelease && obj->objectName() == "imgshowLabel" && globalXY_isPress == 1 && greencolorset == 1) {//object

            str = "MouseButtonPress" + QString::number(pt.x()) + ", " + QString::number(pt.y());
            setWindowTitle(str);                            // 顯示座標
            globalX_release = pt.x();
            globalY_release = pt.y();
            seedStack.push(globalX_release);
            seedStack.push(globalY_release);
            globalXY_isPress = 0;
            //color = Scalar(0,0, 255);//BGR
            circle(mat_import, Point(globalX_release, globalY_release), 1, color, 4);
            //arrowedLine(mat_import, Point(globalX, globalY), Point(globalX_release, globalY_release), color , 3, 8);//BGR
            //imshow("QImage2cvMat RGB24", mat_import);
            update();
        }
        else if (event->type() == QEvent::MouseButtonRelease && obj->objectName() == "imgshowLabel" && globalXY_isPress == 1 && yellowcolorset == 1) {//object_move
            objectX_move = pt.x();
            objectY_move = pt.y();
            globalXY_isPress = 0;
            //circle(mat_import, Point(globalX_move, globalY_move), 1, color, 4);
            arrowedLine(mat_import, Point(globalX, globalY), Point(objectX_move, objectY_move), color, 3, 8);//BGR
            update();
        }
        else if (event->type() == QEvent::MouseButtonRelease && obj->objectName() == "imgshowLabel" && globalXY_isPress == 1 && redcolorset == 1) {//background
            temp_x = pt.x();
            temp_y = pt.y();
            backgroundStack.push(temp_x);
            backgroundStack.push(temp_y);
            globalXY_isPress = 0;
            circle(mat_import, Point(temp_x, temp_y), 1, color, 4);
            //arrowedLine(mat_import, Point(globalX, globalY), Point(globalX_move, globalY_move), color, 3, 8);//BGR
            update();
        }
        else if (event->type() == QEvent::MouseButtonRelease && obj->objectName() == "imgshowLabel" && globalXY_isPress == 1 && blackcolorset == 1) {//background_move
            backgroundmoveStack.push(globalX);
            backgroundmoveStack.push(globalY);
            backgroundX_move = pt.x();
            backgroundY_move = pt.y();
            backgroundmoveStack.push(backgroundX_move);
            backgroundmoveStack.push(backgroundY_move);
            globalXY_isPress = 0;
            arrowedLine(mat_import, Point(globalX, globalY), Point(backgroundX_move, backgroundY_move), color, 3, 8);//BGR
            update();
        }
        else if (event->type() == QEvent::MouseButtonRelease && obj->objectName() == "imgshowLabel" && globalXY_isPress == 1) {
            globalXY_isPress = 0;
        }
    }
    if (superscaler == 1||superscaler ==2) {
        if(event->type() == QEvent::MouseMove && obj->objectName() == "imgshowLabel" && globalXY_isPress == 1 && greencolorset == 1) {//object

            str = "MouseButtonPress" + QString::number(pt.x()) + ", " + QString::number(pt.y());
            setWindowTitle(str);                            // 顯示座標
            slicobjX_release = pt.x();
            slicobjY_release = pt.y();
            slicobjseedStack.push(slicobjX_release);
            slicobjseedStack.push(slicobjY_release);
            //globalXY_isPress = 0;
            //color = Scalar(0,0, 255);//BGR
            image2.convertToFormat(QImage::Format_RGB888);
            mat_import2 = QImage2cvMat(image2);
            circle(mat_import2, Point(slicobjX_release, slicobjY_release), 1, color, 4);
            image2 = cvMat2QImage(mat_import2);
            ui.imgshowLabel->setPixmap(QPixmap::fromImage(image2));
            //arrowedLine(mat_import, Point(globalX, globalY), Point(globalX_release, globalY_release), color , 3, 8);//BGR
            //imshow("QImage2cvMat RGB24", mat_import);
            //update();
            /*while(event->type() == QEvent::MouseMove)
            {
                
                str = "MouseMove in obj" + QString::number(pt.x()) + ", " + QString::number(pt.y());
                setWindowTitle(str);            // 顯示座標
                update();
            }*/
            objupdatenum = 1;
        }        
        else if (event->type() == QEvent::MouseButtonRelease && obj->objectName() == "imgshowLabel" && globalXY_isPress == 1 && yellowcolorset == 1) {//object_move
            objectX_move = pt.x();
            objectY_move = pt.y();
            globalXY_isPress = 0;
            //circle(mat_import, Point(globalX_move, globalY_move), 1, color, 4);
            image2.convertToFormat(QImage::Format_RGB888);
            mat_import2 = QImage2cvMat(image2);
            arrowedLine(mat_import2, Point(globalX, globalY), Point(objectX_move, objectY_move), color, 3, 8);//BGR
            image2 = cvMat2QImage(mat_import2);
            ui.imgshowLabel->setPixmap(QPixmap::fromImage(image2));
            //update();
        }
        else if (event->type() == QEvent::MouseButtonRelease && obj->objectName() == "imgshowLabel" && globalXY_isPress == 1 && redcolorset == 1) {//background
            temp_x = pt.x();
            temp_y = pt.y();
            backgroundStack.push(temp_x);
            backgroundStack.push(temp_y);
            globalXY_isPress = 0;
            image2.convertToFormat(QImage::Format_RGB888);
            mat_import2 = QImage2cvMat(image2);
            circle(mat_import2, Point(temp_x, temp_y), 1, color, 4);
            image2.convertToFormat(QImage::Format_RGB888);
            mat_import2 = QImage2cvMat(image2);
            //arrowedLine(mat_import, Point(globalX, globalY), Point(globalX_move, globalY_move), color, 3, 8);//BGR
            //update();
        }
        else if (event->type() == QEvent::MouseButtonRelease && obj->objectName() == "imgshowLabel" && globalXY_isPress == 1 && blackcolorset == 1) {//background_move
            backgroundmoveStack.push(globalX);
            backgroundmoveStack.push(globalY);
            backgroundX_move = pt.x();
            backgroundY_move = pt.y();
            backgroundmoveStack.push(backgroundX_move);
            backgroundmoveStack.push(backgroundY_move);
            globalXY_isPress = 0;
            mat_import2 = QImage2cvMat(image2);
            circle(mat_import2, Point(temp_x, temp_y), 1, color, 4);
            arrowedLine(mat_import2, Point(globalX, globalY), Point(backgroundX_move, backgroundY_move), color, 3, 8);//BGR
            image2.convertToFormat(QImage::Format_RGB888);
            mat_import2 = QImage2cvMat(image2);
            //update();
        }
        else if (event->type() == QEvent::MouseButtonRelease && obj->objectName() == "imgshowLabel" && globalXY_isPress == 1) {
            globalXY_isPress = 0;
        }
        if (event->type() == QEvent::MouseButtonRelease && obj->objectName() == "imgshowLabel" && globalXY_isPress == 1 && greencolorset == 1 && objupdatenum == 1) {

            stack <int> slicobjseedStack_copy = slicobjseedStack;
            int before_y = slicobjseedStack_copy.top();
            slicobjseedStack_copy.pop();
            int before_x = slicobjseedStack_copy.top();
            slicobjseedStack_copy = slicobjseedStack;
            while (!slicobjseedStack_copy.empty()) {

                int temp_y = slicobjseedStack_copy.top();
                slicobjseedStack_copy.pop();
                int temp_x = slicobjseedStack_copy.top();
                slicobjseedStack_copy.pop();
                //arrowedLine(mat_import, Point(before_x, before_y), Point(temp_x, temp_y), color, 1, 8);//BGR
                circle(mat_import, Point(globalX_release, globalY_release), 1, color, 2);
            }
            objupdatenum = 0;
            globalXY_isPress = 0;
        }
    }
    //if (event->type() == QEvent::MouseMove)
    //{
    //    mouseEvent = static_cast<QMouseEvent*>(event);
    //    str = "MouseMove" + QString::number(pt.x()) + ", " + QString::number(pt.y());
    //    setWindowTitle(str);                            // 顯示座標
    //}
    return false;

}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//







#include "function.h"

int rect_dilate(Mat input, Rect& ROI_rect) {
    //rect 放大
    int maxX = ROI_rect.x + ROI_rect.width;
    int maxY = ROI_rect.y + ROI_rect.height;
    int minX = ROI_rect.x;
    int minY = ROI_rect.y;
    int width;
    int height;
    if (maxX + 5 > input.cols)
        maxX = input.cols;
    else
        maxX = maxX + 5;
    if (maxY + 5 > input.rows)
        maxY = input.rows;
    else
        maxY = maxY + 5;
    if (minX - 5 < 0)
        minX = 0;
    else
        minX = minX - 5;
    if (minY - 5 < 0)
        minY = 0;
    else
        minY = minY - 5;

    width = maxX - minX;
    height = maxY - minY;

    ROI_rect.x = minX;
    ROI_rect.y = minY;
    ROI_rect.width = width;
    ROI_rect.height = height;
    return 0;
}
Mat Trace(Mat input, Point seed, Rect& ROI_rect) {
    Mat src_erode, src;
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
    erode(input, src_erode, element);
    dilate(src_erode, src, element);
    Mat gray;
    Mat src_BI;
    cvtColor(src, gray, COLOR_BGR2HSV);
    //cvtColor(src_BI, gray, COLOR_RGB2GRAY);
    cvtColor(gray, gray, COLOR_BGR2GRAY);
    threshold(gray, src_BI, 0, 255, THRESH_BINARY | THRESH_OTSU);
    //dilate(src_BI, src_BI, element);
    //erode(src_BI, src_BI, element);

    Mat floodfill;
    floodfill = src_BI.clone();

    floodFill(floodfill, seed, Scalar(128, 128, 128), &ROI_rect, Scalar(7, 7, 7), Scalar(7, 7, 7), 4);

    Mat object = Mat::zeros(floodfill.rows, floodfill.cols, CV_8UC1);
    for (int i = 0; i < floodfill.cols; i++) {
        for (int j = 0; j < floodfill.rows; j++) {
            Point pos = Point(i, j);
            if (floodfill.at<uchar>(pos) == 128)
                object.at<uchar>(pos) = 255;
        }
    }
    dilate(object, object, getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1)));

    //rect_dilate(input, ROI_rect);
    return object;
}
Mat Background(Mat input, Mat object) {
    Mat background;
    Mat mask;
    Mat element = getStructuringElement(MORPH_RECT, Size(15, 15), Point(-1, -1));

    dilate(object, mask, element);
    inpaint(input, mask, background, 3, INPAINT_TELEA);
    //blur(background, background, Size(3, 3), Point(-1, -1), 4);
    return background;
}


void fillHole(const Mat srcimage, Mat& dstimage) {
    Size m_Size = srcimage.size();
    Mat temimage = Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcimage.type());
    srcimage.copyTo(temimage(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
    floodFill(temimage, Point(0, 0), Scalar(255));

    Mat cutImg;
    temimage(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
    dstimage = srcimage | (~cutImg);
}
Mat segment(int pointnum,Point bound[], Mat background) {
    //flow = Mat::zeros(input.rows, input.cols, CV_8UC3);
    Mat mask = Mat::zeros(background.rows, background.cols, CV_8UC1);
    //int num = 5;
    for (int i = 0; i < pointnum; i++) {
        if (i < pointnum - 1)
            line(mask, bound[i], bound[i + 1], 255);
        else
            line(mask, bound[i], bound[0], 255);
    }
    fillHole(mask, mask);
    //for (int i = 0; i < input.cols; i++)
    // for (int j = 0; j < input.rows; j++)
    //  if (mask.at<uchar>(Point(i, j)) == 255)
    //   flow.at<Vec3b>(Point(i, j)) = input.at<Vec3b>(Point(i, j));
    return mask;
}
int Savevideo2(Mat input, stack <int> seedStack, stack <int> backgroundStack, stack<int> backgroundmoveStack, int globalX_move, int globalY_move, int frame, char c) {//Mat input, stack <int> seedStack, int globalX_move, int globalY_move, int frame, char c = 10

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    stack <int> seedStack_copy = seedStack;
    Rect ROI_rect;
    int globalY_release = 0, globalX_release = 0;
    if (!seedStack.empty()) {
        globalY_release = seedStack.top();
        seedStack.pop();
        globalX_release = seedStack.top();
        seedStack.pop();
    }
    seedStack = seedStack_copy;

    Point seed = Point(globalX_release, globalY_release);
    Mat object = Mat::zeros(input.rows, input.cols, CV_8UC1);
    Mat ROI;
    Mat mask;
    seedStack = seedStack_copy;
    while (!seedStack.empty())
    {
        Mat object_temp;
        globalY_release = seedStack.top();
        seedStack.pop();
        globalX_release = seedStack.top();
        seedStack.pop();
        seed = Point(globalX_release, globalY_release);
        object_temp = Trace(input, seed, ROI_rect);
        //object = Trace(input, seed, ROI_rect);
        ROI = input(ROI_rect);
        mask = object_temp(ROI_rect);
        int rowNumber = object.rows;
        int colNumber = object.cols;
        for (int i = 0; i < rowNumber; i++)
        {
            for (int j = 0; j < colNumber; j++)
            {
                object.at<uchar>(i, j) = (object.at<uchar>(i, j) | object_temp.at<uchar>(i, j));
            }
        }
    }
    Mat background = Background(input, object);
    movestep move;
    int mask_left = 100000, mask_right = 0, mask_top = 100000, mask_bottom = 0;
    for (int i = 0; i < (object.rows); i++) {
        for (int j = 0; j < (object.cols); j++) {
            if (object.at<uchar>(i, j) == 255 && i < mask_top) { mask_top = i; }
            if (object.at<uchar>(i, j) == 255 && i > mask_bottom) { mask_bottom = i; }
            if (object.at<uchar>(i, j) == 255 && j < mask_left) { mask_left = j; }
            if (object.at<uchar>(i, j) == 255 && j > mask_right) { mask_right = j; }
        }
    }
    seedStack = seedStack_copy;
    Rect newROI_rect;
    int currentX = 0;
    int currentY = 0;
    Mat new_mask;
    Mat new_ROI;
    if (!seedStack.empty()) {
        newROI_rect = Rect(mask_left, mask_top, mask_right - mask_left, mask_bottom - mask_top);
        new_ROI = input(newROI_rect);
        //mask = object_temp(ROI_rect);
        //Mat new_mask = Mat::zeros(mask_bottom - mask_top, mask_right - mask_left, CV_8UC1);
        new_mask = object(newROI_rect);
        currentX = newROI_rect.x;
        currentY = newROI_rect.y;
    }
    //Rect newROI_rect = Rect(mask_left + 2, mask_top + 2, mask_right - mask_left - 3, mask_bottom - mask_top - 3);
    //Mat new_ROI = input(newROI_rect);
    ////mask = object_temp(ROI_rect);
    ////Mat new_mask = Mat::zeros(mask_bottom - mask_top, mask_right - mask_left, CV_8UC1);
    //Mat new_mask = object(newROI_rect);
    Mat seg;


    stack <int> backgroundmoveStack_copy;
    backgroundmoveStack_copy = backgroundmoveStack;
    stack <int> backgroundStack_copy;
    backgroundStack_copy = backgroundStack;
    Mat mask_background;
    int backgroundmovepoint_num = 0;
    int backgroundpoint_num = 0;
    if (!backgroundStack.empty()) {


        while (!backgroundStack.empty()) {
            backgroundStack.pop();
            backgroundStack.pop();
            backgroundpoint_num++;
        }
    }
    Point* bound = new Point[backgroundpoint_num];
    backgroundStack = backgroundStack_copy;
    if (!backgroundStack.empty()) {
        backgroundStack = backgroundStack_copy;
        for (int i = 0; i < backgroundpoint_num; i++) {
            bound[i].y = backgroundStack.top();
            backgroundStack.pop();
            bound[i].x = backgroundStack.top();
            backgroundStack.pop();
        }



        while (!backgroundmoveStack.empty()) {
            backgroundmoveStack.pop();
            backgroundmoveStack.pop();
            backgroundmoveStack.pop();
            backgroundmoveStack.pop();
            backgroundmovepoint_num++;
        }
    }
    Point* start = new Point[backgroundmovepoint_num];
    Point* end = new Point[backgroundmovepoint_num];

    int size = backgroundmovepoint_num; //size = 箭頭總數
    backgroundmoveStack = backgroundmoveStack_copy;
    if (!backgroundmoveStack.empty()) {

        backgroundmoveStack = backgroundmoveStack_copy;
        for (int i = 0; i < backgroundmovepoint_num; i++) {
            end[i].y = backgroundmoveStack.top();
            backgroundmoveStack.pop();
            end[i].x = backgroundmoveStack.top();
            backgroundmoveStack.pop();
            start[i].y = backgroundmoveStack.top();
            backgroundmoveStack.pop();
            start[i].x = backgroundmoveStack.top();
            backgroundmoveStack.pop();
        }
    }
    backgroundmoveStack = backgroundmoveStack_copy;
    if (!backgroundmoveStack.empty()) {
        //Mat background;
        //background = input.clone();
        mask_background = segment(backgroundpoint_num, bound, background);
        seg = block(background, mask_background, start, end, size);

    }
    movestep* dynamic_dist = new movestep[size];
    movestep* dynamic_rate = new movestep[size];
    backgroundmoveStack = backgroundmoveStack_copy;
    if (!backgroundStack_copy.empty()) {
        for (int num = 0; num < size; num++) {
            dynamic_rate[num].x = (end[num].x - start[num].x) / frame;
            dynamic_rate[num].y = (end[num].y - start[num].y) / frame;
        }
        for (int num = 0; num < size; num++) {
            dynamic_dist[num].x = 0;
            dynamic_dist[num].y = 0;
        }
    }
    //...
    VideoWriter writer;

    writer.open("output_origin.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 5.0, Size(background.cols, background.rows), true);
    
    Mat motion;
    int iter = 0;
    int dist_x = 0;
    int dist_y = 0;

    //movestep dynamic_dist;
    move.x = (globalX_move - globalX_release) / (frame);
    move.y = (globalY_move - globalY_release) / (frame);
    seedStack = seedStack_copy;




    while (iter <= frame)
    {
        if (backgroundmoveStack_copy.empty()) { motion = background.clone(); }
        else {
            motion = dynamic(background, seg, size, dynamic_dist);
            //blur(background, background, Size(3, 3), Point(-1, -1), 4);
        }

        if (!seedStack.empty()) {
            for (int i = 0; i < (newROI_rect.width); i++) {
                for (int j = 0; j < (newROI_rect.height); j++) {
                    bool block = (i + currentX) > 0 && (j + currentY) > 0 && (i + currentX) < background.cols && (j + currentY) < background.rows;
                    if (new_mask.at<uchar>(Point(i, j)) == 255 && block)
                        motion.at<Vec3b>(Point((i + currentX), (j + currentY))) = new_ROI.at<Vec3b>(Point(i, j));
                }
            }
            currentX += move.x;
            currentY += move.y;
        }

        Mat frame;
        motion.copyTo(frame);
        writer.write(frame);
        iter++;
        if (!backgroundmoveStack_copy.empty()) {
            for (int num = 0; num < size; num++) {
                dynamic_dist[num].x += dynamic_rate[num].x;
                dynamic_dist[num].y += dynamic_rate[num].y;
            }
        }
        


    }
    seedStack = seedStack_copy;
    backgroundStack = backgroundStack_copy;
    backgroundmoveStack = backgroundmoveStack_copy;
    //destroyAllWindows();
    return 0;
}
int Videoplayer2(Mat input, stack <int> seedStack, stack <int> backgroundStack, stack<int> backgroundmoveStack, int globalX_move, int globalY_move, int frame, char* fileName,char c) {//Mat input, stack <int> seedStack, int globalX_move, int globalY_move, int frame, char c = 10
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    stack <int> seedStack_copy = seedStack;
    Rect ROI_rect;
    int globalY_release=0, globalX_release=0;
    if (!seedStack.empty()) {
        globalY_release = seedStack.top();
        seedStack.pop();
        globalX_release = seedStack.top();
        seedStack.pop();
    }
    seedStack = seedStack_copy;

    Point seed = Point(globalX_release, globalY_release);
    Mat object = Mat::zeros(input.rows, input.cols, CV_8UC1);
    Mat ROI;
    Mat mask;
    seedStack = seedStack_copy;
    while (!seedStack.empty())
    {
        Mat object_temp;
        globalY_release = seedStack.top();
        seedStack.pop();
        globalX_release = seedStack.top();
        seedStack.pop();
        seed = Point(globalX_release, globalY_release);
        object_temp = Trace(input, seed, ROI_rect);
        //object = Trace(input, seed, ROI_rect);
        ROI = input(ROI_rect);
        mask = object_temp(ROI_rect);
        int rowNumber = object.rows;
        int colNumber = object.cols;
        for (int i = 0; i < rowNumber; i++)
        {
            for (int j = 0; j < colNumber; j++)
            {
                object.at<uchar>(i, j) = (object.at<uchar>(i, j) | object_temp.at<uchar>(i, j));
            }
        }
    }
    Mat background = Background(input, object);
    movestep move;
    int mask_left = 100000, mask_right = 0, mask_top = 100000, mask_bottom = 0;
    for (int i = 0; i < (object.rows); i++) {
        for (int j = 0; j < (object.cols); j++) {
            if (object.at<uchar>(i, j) == 255 && i < mask_top) { mask_top = i; }
            if (object.at<uchar>(i, j) == 255 && i > mask_bottom) { mask_bottom = i; }
            if (object.at<uchar>(i, j) == 255 && j < mask_left) { mask_left = j; }
            if (object.at<uchar>(i, j) == 255 && j > mask_right) { mask_right = j; }
        }
    }
    seedStack = seedStack_copy; 
    Rect newROI_rect;
    int currentX = 0;
    int currentY = 0;
    Mat new_mask;
    Mat new_ROI;
    if (!seedStack.empty()) {
        newROI_rect = Rect(mask_left , mask_top , mask_right - mask_left , mask_bottom - mask_top );
        new_ROI = input(newROI_rect);
        //mask = object_temp(ROI_rect);
        //Mat new_mask = Mat::zeros(mask_bottom - mask_top, mask_right - mask_left, CV_8UC1);
        new_mask = object(newROI_rect);
        currentX = newROI_rect.x;
        currentY = newROI_rect.y;
    }
    //Rect newROI_rect = Rect(mask_left + 2, mask_top + 2, mask_right - mask_left - 3, mask_bottom - mask_top - 3);
    //Mat new_ROI = input(newROI_rect);
    ////mask = object_temp(ROI_rect);
    ////Mat new_mask = Mat::zeros(mask_bottom - mask_top, mask_right - mask_left, CV_8UC1);
    //Mat new_mask = object(newROI_rect);
    Mat seg;
    
    
    stack <int> backgroundmoveStack_copy;
    backgroundmoveStack_copy = backgroundmoveStack;
    stack <int> backgroundStack_copy;
    backgroundStack_copy = backgroundStack;
    Mat mask_background;
    int backgroundmovepoint_num = 0;
    int backgroundpoint_num = 0;
    if (!backgroundStack.empty()) {

        
        while (!backgroundStack.empty()) {
            backgroundStack.pop();
            backgroundStack.pop();
            backgroundpoint_num++;
        }
    }
    Point* bound = new Point[backgroundpoint_num];
    backgroundStack = backgroundStack_copy;
    if (!backgroundStack.empty()) {        
        backgroundStack = backgroundStack_copy;
        for (int i = 0; i < backgroundpoint_num; i++) {
            bound[i].y = backgroundStack.top();
            backgroundStack.pop();
            bound[i].x = backgroundStack.top();
            backgroundStack.pop();
        }


       
        while (!backgroundmoveStack.empty()) {
            backgroundmoveStack.pop();
            backgroundmoveStack.pop();
            backgroundmoveStack.pop();
            backgroundmoveStack.pop();
            backgroundmovepoint_num++;
        }
    }
    Point* start = new Point[backgroundmovepoint_num];
    Point* end = new Point[backgroundmovepoint_num];
    
    int size = backgroundmovepoint_num; //size = 箭頭總數
    backgroundmoveStack = backgroundmoveStack_copy;
    if (!backgroundmoveStack.empty()) {

        backgroundmoveStack = backgroundmoveStack_copy;
        for (int i = 0; i < backgroundmovepoint_num; i++) {
            end[i].y = backgroundmoveStack.top();
            backgroundmoveStack.pop();
            end[i].x = backgroundmoveStack.top();
            backgroundmoveStack.pop();
            start[i].y = backgroundmoveStack.top();
            backgroundmoveStack.pop();
            start[i].x = backgroundmoveStack.top();
            backgroundmoveStack.pop();
        }
    }
    backgroundmoveStack = backgroundmoveStack_copy;
    if (!backgroundmoveStack.empty()) {
        //Mat background;
        //background = input.clone();
        mask_background = segment(backgroundpoint_num, bound, background);
        //imwrite("1.jpg", mask_background);
        seg = block(background, mask_background, start, end, size);             
        //imwrite("2.jpg", seg);
    }
    movestep* dynamic_dist = new movestep[size];
    movestep* dynamic_rate = new movestep[size];
    backgroundmoveStack = backgroundmoveStack_copy;
    if (!backgroundStack_copy.empty()) {
        for (int num = 0; num < size; num++) {
            dynamic_rate[num].x = (end[num].x - start[num].x) / frame;
            dynamic_rate[num].y = (end[num].y - start[num].y) / frame;
        }
        for (int num = 0; num < size; num++) {
            dynamic_dist[num].x = 0;
            dynamic_dist[num].y = 0;
        }
    }
    Mat motion;
    int iter = 0;
    int dist_x = 0;
    int dist_y = 0;
    
    //movestep dynamic_dist;
    //move.x = (globalX_move - globalX_release) / (frame);
    //move.y = (globalY_move - globalY_release) / (frame);
    seedStack = seedStack_copy;
    /*double a;
    if ((globalX_move - globalX_release) >= (globalY_move - globalY_release))
    {
        a = (globalX_move - globalX_release) / frame;
        move.x = (frame);
        move.y = globalY_move - globalY_release / (a);
    }
    else {
        a = (globalY_move - globalY_release) / frame;
        move.x = (globalX_move - globalX_release) / a;
        move.y = frame;
    }*/
    
        
   
    /////////////////
    //move.x = (globalX_move - globalX_release) / (frame);
    //move.y = (globalY_move - globalY_release) / (frame);
    int b = 0;
    //int gg = globalX_move - globalX_release;
    //int gg2 = globalY_move - globalY_release;
    if (abs(globalX_move - globalX_release) > abs(globalY_move - globalY_release)) {
        
        b = (globalX_move - globalX_release) * 1.0 / (frame);
    }
    else {
        b = (globalY_move - globalY_release) * 1.0 / (frame);
    }
    if (b < 1) { b = b*-1; }
    else if (b == 0&& (globalX_move - globalX_release)<0) { b = -1; }
    else if (b == 0 && (globalX_move - globalX_release) > 0) { b = 1; }
    //else if (b < 1 && b < 0) { b = -1; }
    double move_x = (globalX_move - globalX_release) * 1.0 / b;
    double move_y = (globalY_move - globalY_release) * 1.0 / b ;
    //if (b < 10) { b = 10; }
    //double rate = sqrt(pow((globalX_move - globalX_release), 2) + pow((globalY_move - globalY_release), 2));
    double temp_movex = 0;
    double temp_movey = 0;
    VideoWriter writer;
    writer.open(fileName, VideoWriter::fourcc('M', 'J', 'P', 'G'), 1.0, Size(background.cols, background.rows), true);
    if (seedStack_copy.empty()) { b = 10; }
    //if (b < 1) { b = 1; }
    while (iter < b)
    {
        if(backgroundmoveStack_copy.empty()){ motion = background.clone(); }
        else { motion = dynamic(background, seg, size, dynamic_dist);
        //blur(background, background, Size(3, 3), Point(-1, -1), 4);
        }
        
        if (!seedStack.empty()) {
            for (int i = 0; i < (newROI_rect.width); i++) {
                for (int j = 0; j < (newROI_rect.height); j++) {
                    bool block = (i + currentX) > 0 && (j + currentY) > 0 && (i + currentX) < background.cols && (j + currentY) < background.rows;
                    if (new_mask.at<uchar>(Point(i, j)) == 255 && block)
                        motion.at<Vec3b>(Point((i + currentX), (j + currentY))) = new_ROI.at<Vec3b>(Point(i, j));
                }
            }
            //temp_movex = move_x + temp_movex;
            //temp_movey = move_y + temp_movey;
            int x = move_x + 0.5;
            int y = move_y + 0.5;
            move.x = x;
            move.y = y;
            currentX += move.x;
            currentY += move.y;
        }
        
        Mat frame;
        motion.copyTo(frame);
        writer.write(frame);
        //imshow("video", frame);
        
        
        //waitKey(1000);
        iter++;
        if (!backgroundmoveStack_copy.empty()) {
            for (int num = 0; num < size; num++) {
                dynamic_dist[num].x += dynamic_rate[num].x;
                dynamic_dist[num].y += dynamic_rate[num].y;
            }
        }
        


    }
    writer.release();
    seedStack = seedStack_copy;
    backgroundStack = backgroundStack_copy;
    backgroundmoveStack = backgroundmoveStack_copy;
    //destroyAllWindows();
    return 0;
}

Mat dynamic(Mat background, Mat seg, int size, movestep* dynamic_dist) {
    int seg_num = 255 / size;
    Point2f srcpoint[3];
    Point2f dstpoint[3];

    Mat dynamic_back = background.clone();
    int level, minX = background.cols, minY = background.rows, maxX = 0, maxY = 0;
    for (int num = 0; num < size; num++) {
        minX = background.cols, minY = background.rows, maxX = 0, maxY = 0;
        if (num == 2)
            num = num;
        level = (num + 1) * seg_num;
        for (int i = 0; i < background.cols; i++)
            for (int j = 0; j < background.rows; j++)
                if (seg.at<uchar>(Point(i, j)) == (uchar)level) {
                    if (i < minX)minX = i;
                    if (i > maxX)maxX = i;
                    if (j < minY)minY = j;
                    if (j > maxY)maxY = j;
                }

        srcpoint[0] = Point2f(minX, minY);
        srcpoint[1] = Point2f(minX, maxY);
        srcpoint[2] = Point2f(maxX, maxY);
        //srcpoint[3] = Point2f(maxX, minY);


        dstpoint[0] = Point2f(minX, minY);
        dstpoint[1] = Point2f(minX, maxY);
        dstpoint[2] = Point2f(maxX, maxY);
        //srcpoint[3] = Point2f(maxX, minY);
        if (dynamic_dist[num].x > 0) {
            dstpoint[2] = Point2f(maxX + dynamic_dist[num].x, maxY);
            //dstpoint[3] = Point2f(maxX + dynamic_dist[num].x, minY);
        }
        else if (dynamic_dist[num].x < 0) {
            dstpoint[0] = Point2f(minX + dynamic_dist[num].x, minY);
            dstpoint[1] = Point2f(minX + dynamic_dist[num].x, maxY);
        }

        if (dynamic_dist[num].y > 0) {
            dstpoint[1] = Point2f(minX, maxY + dynamic_dist[num].y);
            dstpoint[2] = Point2f(maxX, maxY + dynamic_dist[num].y);
        }

        else if (dynamic_dist[num].y < 0) {
            dstpoint[0] = Point2f(minX, minY + dynamic_dist[num].y);
            //dstpoint[3] = Point2f(maxX, minY + dynamic_dist[num].y);

        }
        Mat part = Mat::zeros(background.rows, background.cols, background.type());
        Mat M = getAffineTransform(srcpoint, dstpoint);
        cv::warpAffine(background, part, M, part.size());

        for (int i = 0; i < background.cols; i++) {
            for (int j = 0; j < background.rows; j++) {
                Vec3b t;
                t.val[0] = (uchar)0x00;
                t.val[1] = (uchar)0x00;
                t.val[2] = (uchar)0x00;
                if (seg.at<uchar>(Point(i, j)) == (uchar)level && part.at<Vec3b>(Point(i,j))!=t) {
                    dynamic_back.at<Vec3b>(Point(i, j)) = part.at<Vec3b>(Point(i, j));
                }
            }
        }

    }
    return dynamic_back;
}
Mat block(Mat background, Mat mask, Point start[], Point end[], int size) {

    Mat marker = Mat::zeros(background.rows, background.cols, CV_8UC1);
    Point temp;
    int seg = 255 / size;
    for (int i = 0; i < size; i++) {
        temp.x = (start[i].x + end[i].x) / 2;
        temp.y = (start[i].y + end[i].y) / 2;
        //circle(marker, temp, 3, Scalar(30*(i + 1)), -1);
        circle(marker, Point(start[i].x, start[i].y), 3, Scalar(seg * (i + 1)), -1);
        circle(marker, Point(end[i].x, end[i].y), 3, Scalar(seg * (i + 1)), -1);
        //test.at<uchar>(temp) = 255;
    }

    marker.convertTo(marker, CV_32S);
    watershed(background, marker);

    marker.convertTo(marker, CV_8UC1);
    //Size m_Size = marker.size();
    //Mat temimage = Mat::zeros(m_Size.height + 2, m_Size.width + 2, marker.type());
    //marker.copyTo(temimage(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
    //for (int i = 1; i < marker.cols+2; i++) {
    //    for (int j = 1; j < marker.rows+2; j++) {
    //        if (temimage.at<int>(Point(i, j)) == -1) {
    //            int max = temimage.at<int>(Point(i, j));
    //            for (int c = -1; c < 2; c++) {
    //                for (int r =-1; r < 2; r++) {
    //                    if (temimage.at<int>(Point(i + c, j + r)) > max) {
    //                        max = temimage.at<int>(Point(i + c, j + r));
    //                        
    //                        
    //                    }
    //                }
    //            }
    //            temimage.at<int>(Point(i, j)) = max;
    //        }

    //    }

    //}
    //Mat cutImg;
    //temimage(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
    //cutImg.convertTo(cutImg, CV_8UC1);

    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
    dilate(marker, marker, element);
    erode(marker, marker, element);
    for (int i = 0; i < background.cols; i++)
        for (int j = 0; j < background.rows; j++)
            if (mask.at<uchar>(Point(i, j)) == 0)
                marker.at<uchar>(Point(i, j)) = 0;
    return marker;
}
int Videoplayer3(Mat input, Mat inputslic,Mat slic_black,stack <int> seedStack, stack <int> backgroundStack, stack<int> backgroundmoveStack, int globalX_move, int globalY_move, int frame, char* fileName, char c) {//Mat input, stack <int> seedStack, int globalX_move, int globalY_move, int frame, char c = 10

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    stack <int> seedStack_copy = seedStack;
    Rect ROI_rect;
    int globalY_release = 0, globalX_release = 0;
    
    if (!seedStack.empty()) {
        globalY_release = seedStack.top();
        seedStack.pop();
        globalX_release = seedStack.top();
        seedStack.pop();
    }
    seedStack = seedStack_copy;

    //Point seed = Point(globalX_release, globalY_release);
    Mat object = Mat::zeros(input.rows, input.cols, CV_8UC1);
    //Mat ROI;
    //Mat mask;
    object = SLIC_trace1(input, inputslic, slic_black,seedStack, ROI_rect, 2);
    
    

    int mask_left = 100000, mask_right = 0, mask_top = 100000, mask_bottom = 0;
    for (int i = 0; i < (object.rows); i++) {
        for (int j = 0; j < (object.cols); j++) {
            if (object.at<uchar>(i, j) == 255 && i < mask_top) { mask_top = i; }
            if (object.at<uchar>(i, j) == 255 && i > mask_bottom) { mask_bottom = i; }
            if (object.at<uchar>(i, j) == 255 && j < mask_left) { mask_left = j; }
            if (object.at<uchar>(i, j) == 255 && j > mask_right) { mask_right = j; }
        }
    }
    seedStack = seedStack_copy;
    Rect newROI_rect;
    int currentX = 0;
    int currentY = 0;
    Mat new_mask;
    Mat new_ROI;
    if (!seedStack.empty()) {
        newROI_rect = Rect(mask_left, mask_top, mask_right - mask_left, mask_bottom - mask_top);
        new_ROI = input(newROI_rect);
        //mask = object_temp(ROI_rect);
        //Mat new_mask = Mat::zeros(mask_bottom - mask_top, mask_right - mask_left, CV_8UC1);
        new_mask = object(newROI_rect);
        currentX = newROI_rect.x;
        currentY = newROI_rect.y;
    }
    Mat background = Background(input, object);
    movestep move;
   
    //Rect newROI_rect = Rect(mask_left + 2, mask_top + 2, mask_right - mask_left - 3, mask_bottom - mask_top - 3);
    //Mat new_ROI = input(newROI_rect);
    ////mask = object_temp(ROI_rect);
    ////Mat new_mask = Mat::zeros(mask_bottom - mask_top, mask_right - mask_left, CV_8UC1);
    //Mat new_mask = object(newROI_rect);
    Mat seg;


    stack <int> backgroundmoveStack_copy;
    backgroundmoveStack_copy = backgroundmoveStack;
    stack <int> backgroundStack_copy;
    backgroundStack_copy = backgroundStack;
    Mat mask_background;
    int backgroundmovepoint_num = 0;
    int backgroundpoint_num = 0;
    if (!backgroundStack.empty()) {


        while (!backgroundStack.empty()) {
            backgroundStack.pop();
            backgroundStack.pop();
            backgroundpoint_num++;
        }
    }
    Point* bound = new Point[backgroundpoint_num];
    backgroundStack = backgroundStack_copy;
    if (!backgroundStack.empty()) {
        backgroundStack = backgroundStack_copy;
        for (int i = 0; i < backgroundpoint_num; i++) {
            bound[i].y = backgroundStack.top();
            backgroundStack.pop();
            bound[i].x = backgroundStack.top();
            backgroundStack.pop();
        }



        while (!backgroundmoveStack.empty()) {
            backgroundmoveStack.pop();
            backgroundmoveStack.pop();
            backgroundmoveStack.pop();
            backgroundmoveStack.pop();
            backgroundmovepoint_num++;
        }
    }
    Point* start = new Point[backgroundmovepoint_num];
    Point* end = new Point[backgroundmovepoint_num];

    int size = backgroundmovepoint_num; //size = 箭頭總數
    backgroundmoveStack = backgroundmoveStack_copy;
    if (!backgroundmoveStack.empty()) {

        backgroundmoveStack = backgroundmoveStack_copy;
        for (int i = 0; i < backgroundmovepoint_num; i++) {
            end[i].y = backgroundmoveStack.top();
            backgroundmoveStack.pop();
            end[i].x = backgroundmoveStack.top();
            backgroundmoveStack.pop();
            start[i].y = backgroundmoveStack.top();
            backgroundmoveStack.pop();
            start[i].x = backgroundmoveStack.top();
            backgroundmoveStack.pop();
        }
    }
    backgroundmoveStack = backgroundmoveStack_copy;
    if (!backgroundmoveStack.empty()) {
        //Mat background;
        //background = input.clone();
        mask_background = segment(backgroundpoint_num, bound, background);
        seg = block(background, mask_background, start, end, size);

    }
    movestep* dynamic_dist = new movestep[size];
    movestep* dynamic_rate = new movestep[size];
    backgroundmoveStack = backgroundmoveStack_copy;
    if (!backgroundStack_copy.empty()) {
        for (int num = 0; num < size; num++) {
            dynamic_rate[num].x = (end[num].x - start[num].x) / frame;
            dynamic_rate[num].y = (end[num].y - start[num].y) / frame;
        }
        for (int num = 0; num < size; num++) {
            dynamic_dist[num].x = 0;
            dynamic_dist[num].y = 0;
        }
    }
    Mat motion;
    int iter = 0;
    int dist_x = 0;
    int dist_y = 0;

    //movestep dynamic_dist;
   
    /////////////////
    //move.x = (globalX_move - globalX_release) / (frame);
    //move.y = (globalY_move - globalY_release) / (frame);
    int b = 0;
    if (abs(globalX_move - globalX_release) > abs(globalY_move - globalY_release)) {
        b = (globalX_move - globalX_release) * 1.0 / (frame);
    }
    else {
        b = (globalY_move - globalY_release) * 1.0 / (frame);
    }
    if (b < 1) { b = b * -1; }
    else if (b == 0 && (globalX_move - globalX_release) < 0) { b = -1; }
    else if (b == 0 && (globalX_move - globalX_release) > 0) { b = 1; }
    double move_x = (globalX_move - globalX_release) * 1.0 / b;
    double move_y = (globalY_move - globalY_release) * 1.0 / b ;
    if (seedStack_copy.empty()) { b = 10; }
    if (b < 1) { b = 1; }
    //double rate = sqrt(pow((globalX_move - globalX_release), 2) + pow((globalY_move - globalY_release), 2));
    //double temp_movex = 0;
    //double temp_movey = 0;

    /*if ((globalX_move - globalX_release) >= (globalY_move - globalY_release))
    {
        a = (globalX_move - globalX_release) / frame;
        move.x = (frame);
        
        move.y = (globalY_move - globalY_release )/(a);
        if (move.y == 0&& (globalY_move - globalY_release)>0) {
            move.y = -1;
        }
        else if (move.y == 0 && (globalY_move - globalY_release) < 0) {
            move.y = 1;
        }
    }
    else {
        a = (globalY_move - globalY_release) / frame;
        move.x = (globalX_move - globalX_release) / a;
        if (move.x = 0 && (globalX_move - globalX_release) > 0) {
            move.x = -1;
        }
        else if (move.x = 0 && (globalX_move - globalX_release) < 0) {
            move.x = 1;
        }
        move.y = frame;
    }*/
    //move.x =  (frame);
    //move.y = (frame);
    seedStack = seedStack_copy;



    VideoWriter writer;
    writer.open(fileName, VideoWriter::fourcc('M', 'J', 'P', 'G'), 1.5, Size(background.cols, background.rows), true);
    while (iter < b)
    {
        if (backgroundmoveStack_copy.empty()) { motion = background.clone(); }
        else {
            motion = dynamic(background, seg, size, dynamic_dist);
            //blur(background, background, Size(3, 3), Point(-1, -1), 4);
        }

        if (!seedStack.empty()) {
            for (int i = 0; i < (newROI_rect.width); i++) {
                for (int j = 0; j < (newROI_rect.height); j++) {
                    bool block = (i + currentX) > 0 && (j + currentY) > 0 && (i + currentX) < background.cols && (j + currentY) < background.rows;
                    if (new_mask.at<uchar>(Point(i, j)) == 255 && block)
                        motion.at<Vec3b>(Point((i + currentX), (j + currentY))) = new_ROI.at<Vec3b>(Point(i, j));
                }
            }
            int x = move_x + 0.5;
            int y = move_y + 0.5;
            move.x = x;
            move.y = y;
            currentX += move.x;
            currentY += move.y;
        }

        Mat frame;
        motion.copyTo(frame);
        
        
        //imshow("video", frame);
        writer.write(frame);
        //waitKey(1000);
        iter++;
        if (!backgroundmoveStack_copy.empty()) {
            for (int num = 0; num < size; num++) {
                dynamic_dist[num].x += dynamic_rate[num].x;
                dynamic_dist[num].y += dynamic_rate[num].y;
            }
        }
       

    }
    writer.release();
    seedStack = seedStack_copy;
    backgroundStack = backgroundStack_copy;
    backgroundmoveStack = backgroundmoveStack_copy;
    //destroyAllWindows();
    return 0;
}
int Savevideo3(Mat input, stack <int> seedStack, stack <int> backgroundStack, stack<int> backgroundmoveStack, int globalX_move, int globalY_move, int frame, char c) {//Mat input, stack <int> seedStack, int globalX_move, int globalY_move, int frame, char c = 10

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    stack <int> seedStack_copy = seedStack;
    Rect ROI_rect;
    int globalY_release = 0, globalX_release = 0;

    if (!seedStack.empty()) {
        globalY_release = seedStack.top();
        seedStack.pop();
        globalX_release = seedStack.top();
        seedStack.pop();
    }
    seedStack = seedStack_copy;

    //Point seed = Point(globalX_release, globalY_release);
    Mat object = Mat::zeros(input.rows, input.cols, CV_8UC1);
    //Mat ROI;
    //Mat mask;
    object = SLIC_trace(input, seedStack, ROI_rect, 2);


    int mask_left = 100000, mask_right = 0, mask_top = 100000, mask_bottom = 0;
    for (int i = 0; i < (object.rows); i++) {
        for (int j = 0; j < (object.cols); j++) {
            if (object.at<uchar>(i, j) == 255 && i < mask_top) { mask_top = i; }
            if (object.at<uchar>(i, j) == 255 && i > mask_bottom) { mask_bottom = i; }
            if (object.at<uchar>(i, j) == 255 && j < mask_left) { mask_left = j; }
            if (object.at<uchar>(i, j) == 255 && j > mask_right) { mask_right = j; }
        }
    }
    seedStack = seedStack_copy;
    Rect newROI_rect;
    int currentX = 0;
    int currentY = 0;
    Mat new_mask;
    Mat new_ROI;
    if (!seedStack.empty()) {
        newROI_rect = Rect(mask_left, mask_top, mask_right - mask_left, mask_bottom - mask_top);
        new_ROI = input(newROI_rect);
        //mask = object_temp(ROI_rect);
        //Mat new_mask = Mat::zeros(mask_bottom - mask_top, mask_right - mask_left, CV_8UC1);
        new_mask = object(newROI_rect);
        currentX = newROI_rect.x;
        currentY = newROI_rect.y;
    }
    Mat background = Background(input, object);
    movestep move;

    //Rect newROI_rect = Rect(mask_left + 2, mask_top + 2, mask_right - mask_left - 3, mask_bottom - mask_top - 3);
    //Mat new_ROI = input(newROI_rect);
    ////mask = object_temp(ROI_rect);
    ////Mat new_mask = Mat::zeros(mask_bottom - mask_top, mask_right - mask_left, CV_8UC1);
    //Mat new_mask = object(newROI_rect);
    Mat seg;


    stack <int> backgroundmoveStack_copy;
    backgroundmoveStack_copy = backgroundmoveStack;
    stack <int> backgroundStack_copy;
    backgroundStack_copy = backgroundStack;
    Mat mask_background;
    int backgroundmovepoint_num = 0;
    int backgroundpoint_num = 0;
    if (!backgroundStack.empty()) {


        while (!backgroundStack.empty()) {
            backgroundStack.pop();
            backgroundStack.pop();
            backgroundpoint_num++;
        }
    }
    Point* bound = new Point[backgroundpoint_num];
    backgroundStack = backgroundStack_copy;
    if (!backgroundStack.empty()) {
        backgroundStack = backgroundStack_copy;
        for (int i = 0; i < backgroundpoint_num; i++) {
            bound[i].y = backgroundStack.top();
            backgroundStack.pop();
            bound[i].x = backgroundStack.top();
            backgroundStack.pop();
        }



        while (!backgroundmoveStack.empty()) {
            backgroundmoveStack.pop();
            backgroundmoveStack.pop();
            backgroundmoveStack.pop();
            backgroundmoveStack.pop();
            backgroundmovepoint_num++;
        }
    }
    Point* start = new Point[backgroundmovepoint_num];
    Point* end = new Point[backgroundmovepoint_num];

    int size = backgroundmovepoint_num; //size = 箭頭總數
    backgroundmoveStack = backgroundmoveStack_copy;
    if (!backgroundmoveStack.empty()) {

        backgroundmoveStack = backgroundmoveStack_copy;
        for (int i = 0; i < backgroundmovepoint_num; i++) {
            end[i].y = backgroundmoveStack.top();
            backgroundmoveStack.pop();
            end[i].x = backgroundmoveStack.top();
            backgroundmoveStack.pop();
            start[i].y = backgroundmoveStack.top();
            backgroundmoveStack.pop();
            start[i].x = backgroundmoveStack.top();
            backgroundmoveStack.pop();
        }
    }
    backgroundmoveStack = backgroundmoveStack_copy;
    if (!backgroundmoveStack.empty()) {
        //Mat background;
        //background = input.clone();
        mask_background = segment(backgroundpoint_num, bound, background);
        seg = block(background, mask_background, start, end, size);

    }
    movestep* dynamic_dist = new movestep[size];
    movestep* dynamic_rate = new movestep[size];
    backgroundmoveStack = backgroundmoveStack_copy;
    if (!backgroundStack_copy.empty()) {
        for (int num = 0; num < size; num++) {
            dynamic_rate[num].x = (end[num].x - start[num].x) / frame;
            dynamic_rate[num].y = (end[num].y - start[num].y) / frame;
        }
        for (int num = 0; num < size; num++) {
            dynamic_dist[num].x = 0;
            dynamic_dist[num].y = 0;
        }
    }
    Mat motion;
    int iter = 0;
    int dist_x = 0;
    int dist_y = 0;
    VideoWriter writer;
    writer.open("output_SLIC.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 5.0, Size(background.cols, background.rows), true);
    //movestep dynamic_dist;
    move.x = (globalX_move - globalX_release) / (frame);
    move.y = (globalY_move - globalY_release) / (frame);
    seedStack = seedStack_copy;




    while (iter <= frame)
    {
        if (backgroundmoveStack_copy.empty()) { motion = background.clone(); }
        else {
            motion = dynamic(background, seg, size, dynamic_dist);
            //blur(background, background, Size(3, 3), Point(-1, -1), 4);
        }

        if (!seedStack.empty()) {
            for (int i = 0; i < (newROI_rect.width); i++) {
                for (int j = 0; j < (newROI_rect.height); j++) {
                    bool block = (i + currentX) > 0 && (j + currentY) > 0 && (i + currentX) < background.cols && (j + currentY) < background.rows;
                    if (new_mask.at<uchar>(Point(i, j)) == 255 && block)
                        motion.at<Vec3b>(Point((i + currentX), (j + currentY))) = new_ROI.at<Vec3b>(Point(i, j));
                }
            }
            currentX += move.x;
            currentY += move.y;
        }

        Mat frame;
        motion.copyTo(frame);
        writer.write(frame);
        
        iter++;
        if (!backgroundmoveStack_copy.empty()) {
            for (int num = 0; num < size; num++) {
                dynamic_dist[num].x += dynamic_rate[num].x;
                dynamic_dist[num].y += dynamic_rate[num].y;
            }
        }
        


    }
    seedStack = seedStack_copy;
    backgroundStack = backgroundStack_copy;
    backgroundmoveStack = backgroundmoveStack_copy;
    //destroyAllWindows();
    return 0;
}
Mat SLIC_trace(Mat src, stack<int> test , Rect& ROI_rect,int superscaler)//Mat input, Point seed, Rect& ROI_rect
{
    //clock_t start;
    //clock_t end;
    Mat input = src.clone();
    Mat labels;
    Mat input_copy;
    input.copyTo(input_copy);
    Mat mask;
    


    
    //start = clock();//开始计时

    Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(input, 101, 10, 1.0);//创建一个对象

    slic->iterate(50);//迭代次数，默认为10
    slic->enforceLabelConnectivity();
    slic->getLabelContourMask(mask);//获取超像素的边界
    slic->getLabels(labels);//获取labels
    int number = slic->getNumberOfSuperpixels();//获取超像素的数量

    input.setTo(Scalar(255, 255, 255), mask);
    //end = clock();//结束计时
    //cout << "时间:" << end - start << endl;
    

    if (superscaler == 1) {
        Point a;
        //stack<int> test;    // 堆疊
        //test.push(516);//x
        //test.push(105);//y
        //test.push(202);//x
        //test.push(61);//y
        //test.push(195);//x
        //test.push(82);//y
        //test.push(199);//x
        //test.push(71);//y
        stack<int> test_copy;
        test_copy = test;
        Mat mask_copy;
        mask.copyTo(mask_copy);
        Point neibor(1, 2);
        stack<int> test_temp;
        Point neibor_check1;
        Point neibor_check2;
        Point neibor_check3;
        test_temp = test;
        int see_x = mask.cols;
        int see_y = mask.rows;
        int check = 0;
        while (!test.empty())
        {
            a.y = test.top();
            test.pop();
            a.x = test.top();
            test.pop();
            int x = a.x;
            int y = a.y;
            if (mask.at<uchar>(a) != 255) {
                mask.at<uchar>(a) = 255;
                check = 0;

                if (x + 1 >= 0) {
                    neibor = Point(x + 1, y);//右
                    if (y + 1 <= mask.rows && y - 1 >= 0) {
                        neibor_check1 = Point(x + 1, y + 1);//右下
                        neibor_check2 = Point(x + 1, y - 1);//右上
                        if (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255)
                        {
                            check = 1;
                        }
                        else check = 0;
                    }
                    if ((mask.at<uchar>(neibor) == 0) && ((x + 1) < mask.cols) && y <= mask.rows && check == 0)
                    {
                        test.push(x + 1);
                        test.push(y);
                        test_temp.push(x + 1);
                        test_temp.push(y);
                    }
                    else {
                        test_temp.push(x + 1);
                        test_temp.push(y);
                    }
                }
                if (x - 1 >= 0) {
                    neibor = Point(x - 1, y);//左
                    if (y + 1 <= mask.rows && y - 1 >= 0) {
                        neibor_check1 = Point(x - 1, y + 1);//左下
                        neibor_check2 = Point(x - 1, y - 1);//左上
                        if (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255)
                        {
                            check = 1;
                        }
                        else check = 0;
                    }
                    if ((mask.at<uchar>(neibor) == 0) && ((x - 1) < mask.cols) && y <= mask.rows && check == 0)
                    {
                        test.push(x - 1);
                        test.push(y);
                        test_temp.push(x - 1);
                        test_temp.push(y);
                    }
                    else {
                        test_temp.push(x - 1);
                        test_temp.push(y);
                    }
                }
                if (y + 1 <= mask.rows) {
                    neibor = Point(x, y + 1);//下
                    if (x + 1 <= mask.cols && x - 1 >= 0) {
                        neibor_check1 = Point(x - 1, y + 1);//下左
                        neibor_check2 = Point(x + 1, y + 1);//下右
                        neibor_check3 = Point(x + 1, y);//右
                        if ((mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255) || (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check3) == 255))
                        {
                            check = 1;
                        }
                        else check = 0;


                    }

                    if ((mask.at<uchar>(neibor) == 0) && (x < mask.cols) && (y + 1) >= 0 && check == 0)
                    {

                        test.push(x);
                        test.push(y + 1);
                        test_temp.push(x);
                        test_temp.push(y + 1);
                    }
                    else {
                        test_temp.push(x);
                        test_temp.push(y + 1);
                    }
                }
                if (y - 1 >= 0) {
                    neibor = Point(x, y - 1);//上
                    if (x + 1 <= mask.cols && x - 1 >= 0) {
                        neibor_check1 = Point(x - 1, y - 1);//上左
                        neibor_check2 = Point(x + 1, y - 1);//上右					
                        neibor_check3 = Point(x - 1, y);//左
                        if ((mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255) || (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check3) == 255) || (mask.at<uchar>(neibor_check2) == 255 && mask.at<uchar>(neibor_check3) == 255))
                        {
                            check = 1;
                        }
                        else check = 0;
                    }
                    if ((mask.at<uchar>(neibor) == 0) && (x < mask.cols) && (y - 1) <= mask.rows && check == 0)
                    {
                        test.push(x);
                        test.push(y - 1);
                        test_temp.push(x);
                        test_temp.push(y - 1);
                    }
                    else {
                        test_temp.push(x);
                        test_temp.push(y - 1);
                    }
                }
            }

        }

        //fillHole(mask, mask);
        Mat mask_2 = Mat::zeros(mask.rows, mask.cols, CV_8UC1);;

        while (!test_temp.empty()) {
            int temp_y = test_temp.top();
            test_temp.pop();
            int temp_x = test_temp.top();
            test_temp.pop();
            Point temp(temp_x, temp_y);
            mask_2.at<uchar>(temp) = 255;
            fillHole(mask_2, mask_2);
            for (int i = 0; i < input.cols; i++) {
                for (int j = 0; j < input.rows; j++) {
                    if(mask_2.at<uchar>(Point(i,j))==(uchar)255)
                        input.at<Vec3b>(Point(i, j)) = (255,255,255);

                }
            }
        }

        //fillHole(input, input);
       /* Mat floodfill;
        floodfill = input_copy.clone();

        floodFill(floodfill, seed, Scalar(128, 128, 128), &ROI_rect, Scalar(7, 7, 7), Scalar(7, 7, 7), 4);

        Mat object = Mat::zeros(floodfill.rows, floodfill.cols, CV_8UC1);
        for (int i = 0; i < floodfill.cols; i++) {
            for (int j = 0; j < floodfill.rows; j++) {
                Point pos = Point(i, j);
                if (floodfill.at<uchar>(pos) == 128)
                    object.at<uchar>(pos) = 255;
            }
        }
       
        return object;*/
        test = test_copy;
        return input;
    }
    else if (superscaler == 2) {
        Point a;
        //stack<int> test;    // 堆疊
        //test.push(516);//x
        //test.push(105);//y
        //test.push(202);//x
        //test.push(61);//y
        //test.push(195);//x
        //test.push(82);//y
        //test.push(199);//x
        //test.push(71);//y
        stack<int> test_copy;
        test_copy = test;
        Mat mask_copy;
        mask.copyTo(mask_copy);
        Point neibor(1, 2);
        stack<int> test_temp;
        Point neibor_check1;
        Point neibor_check2;
        Point neibor_check3;
        test_temp = test;
        int see_x = mask.cols;
        int see_y = mask.rows;
        int check = 0;
        while (!test.empty())
        {
            a.y = test.top();
            test.pop();
            a.x = test.top();
            test.pop();
            int x = a.x;
            int y = a.y;
            if (mask.at<uchar>(a) != 255) {
                mask.at<uchar>(a) = 255;
                check = 0;

                if (x + 1 >= 0) {
                    neibor = Point(x + 1, y);//右
                    if (y + 1 <= mask.rows && y - 1 >= 0) {
                        neibor_check1 = Point(x + 1, y + 1);//右下
                        neibor_check2 = Point(x + 1, y - 1);//右上
                        if (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255)
                        {
                            check = 1;
                        }
                        else check = 0;
                    }
                    if ((mask.at<uchar>(neibor) == 0) && ((x + 1) < mask.cols) && y <= mask.rows && check == 0)
                    {
                        test.push(x + 1);
                        test.push(y);
                        test_temp.push(x + 1);
                        test_temp.push(y);
                    }
                    else {
                        test_temp.push(x + 1);
                        test_temp.push(y);
                    }
                }
                if (x - 1 >= 0) {
                    neibor = Point(x - 1, y);//左
                    if (y + 1 <= mask.rows && y - 1 >= 0) {
                        neibor_check1 = Point(x - 1, y + 1);//左下
                        neibor_check2 = Point(x - 1, y - 1);//左上
                        if (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255)
                        {
                            check = 1;
                        }
                        else check = 0;
                    }
                    if ((mask.at<uchar>(neibor) == 0) && ((x - 1) < mask.cols) && y <= mask.rows && check == 0)
                    {
                        test.push(x - 1);
                        test.push(y);
                        test_temp.push(x - 1);
                        test_temp.push(y);
                    }
                    else {
                        test_temp.push(x - 1);
                        test_temp.push(y);
                    }
                }
                if (y + 1 <= mask.rows) {
                    neibor = Point(x, y + 1);//下
                    if (x + 1 <= mask.cols && x - 1 >= 0) {
                        neibor_check1 = Point(x - 1, y + 1);//下左
                        neibor_check2 = Point(x + 1, y + 1);//下右
                        neibor_check3 = Point(x + 1, y);//右
                        if ((mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255) || (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check3) == 255))
                        {
                            check = 1;
                        }
                        else check = 0;


                    }

                    if ((mask.at<uchar>(neibor) == 0) && (x < mask.cols) && (y + 1) >= 0 && check == 0)
                    {

                        test.push(x);
                        test.push(y + 1);
                        test_temp.push(x);
                        test_temp.push(y + 1);
                    }
                    else {
                        test_temp.push(x);
                        test_temp.push(y + 1);
                    }
                }
                if (y - 1 >= 0) {
                    neibor = Point(x, y - 1);//上
                    if (x + 1 <= mask.cols && x - 1 >= 0) {
                        neibor_check1 = Point(x - 1, y - 1);//上左
                        neibor_check2 = Point(x + 1, y - 1);//上右					
                        neibor_check3 = Point(x - 1, y);//左
                        if ((mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255) || (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check3) == 255) || (mask.at<uchar>(neibor_check2) == 255 && mask.at<uchar>(neibor_check3) == 255))
                        {
                            check = 1;
                        }
                        else check = 0;
                    }
                    if ((mask.at<uchar>(neibor) == 0) && (x < mask.cols) && (y - 1) <= mask.rows && check == 0)
                    {
                        test.push(x);
                        test.push(y - 1);
                        test_temp.push(x);
                        test_temp.push(y - 1);
                    }
                    else {
                        test_temp.push(x);
                        test_temp.push(y - 1);
                    }
                }
            }

        }
        Mat mask_2 = Mat::zeros(mask.rows, mask.cols, CV_8UC1);;

        while (!test_temp.empty()) {
            int temp_y = test_temp.top();
            test_temp.pop();
            int temp_x = test_temp.top();
            test_temp.pop();
            Point temp(temp_x, temp_y);
            mask_2.at<uchar>(temp) = 255;
            fillHole(mask_2, mask_2);
            
        }
        /* Mat floodfill;
         floodfill = input_copy.clone();

         floodFill(floodfill, seed, Scalar(128, 128, 128), &ROI_rect, Scalar(7, 7, 7), Scalar(7, 7, 7), 4);

         Mat object = Mat::zeros(floodfill.rows, floodfill.cols, CV_8UC1);
         for (int i = 0; i < floodfill.cols; i++) {
             for (int j = 0; j < floodfill.rows; j++) {
                 Point pos = Point(i, j);
                 if (floodfill.at<uchar>(pos) == 128)
                     object.at<uchar>(pos) = 255;
             }
         }

         return object;*/
        //fillHole(mask_2, mask_2);
        test = test_copy;
        return mask_2;
    }
    
    else { return input; }



}
Mat SLIC_trace1(Mat src, Mat inuptslic, Mat& slic_black, stack<int> test, Rect& ROI_rect, int superscaler)//Mat input, Point seed, Rect& ROI_rect
{
    //clock_t start;
    //clock_t end;
    Mat labels;
    Mat input_copy;
    Mat mask;
    Mat input;
    if(superscaler==0){
        input = src.clone();
        
        input.copyTo(input_copy);
        
        Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(input, 101, 10, 1.0);//创建一个对象

        slic->iterate(50);//迭代次数，默认为10
        slic->enforceLabelConnectivity();
        slic->getLabelContourMask(mask);//获取超像素的边界
        slic->getLabels(labels);//获取labels
        int number = slic->getNumberOfSuperpixels();//获取超像素的数量

        input.setTo(Scalar(255, 255, 255), mask);
        slic_black = mask.clone();
        //Mat input_666;
        //end = clock();//结束计时
        //cout << "时间:" << end - start << endl;
    }
    else {
        input = inuptslic.clone();
        input.copyTo(input_copy);        
    }
    
    
    //start = clock();//开始计时

   
    stack<int> test_copy;
    mask = slic_black.clone();
    if (superscaler == 1) {
        Point a;
        //stack<int> test;    // 堆疊
        //test.push(516);//x
        //test.push(105);//y
        //test.push(202);//x
        //test.push(61);//y
        //test.push(195);//x
        //test.push(82);//y
        //test.push(199);//x
        //test.push(71);//y
        
        test_copy = test;
        Mat mask_copy;
        mask.copyTo(mask_copy);
        Point neibor(1, 2);
        stack<int> test_temp;
        Point neibor_check1;
        Point neibor_check2;
        Point neibor_check3;
        test_temp = test;
        int see_x = mask.cols;
        int see_y = mask.rows;
        int check = 0;
        while (!test.empty())
        {
            a.y = test.top();
            test.pop();
            a.x = test.top();
            test.pop();
            int x = a.x;
            int y = a.y;
            if (mask.at<uchar>(a) != 255) {
                mask.at<uchar>(a) = 255;
                check = 0;

                if (x + 1 >= 0) {
                    neibor = Point(x + 1, y);//右
                    if (y + 1 <= mask.rows && y - 1 >= 0) {
                        neibor_check1 = Point(x + 1, y + 1);//右下
                        neibor_check2 = Point(x + 1, y - 1);//右上
                        if (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255)
                        {
                            check = 1;
                        }
                        else check = 0;
                    }
                    if ((mask.at<uchar>(neibor) == 0) && ((x + 1) < mask.cols) && y <= mask.rows && check == 0)
                    {
                        test.push(x + 1);
                        test.push(y);
                        test_temp.push(x + 1);
                        test_temp.push(y);
                    }
                    else {
                        test_temp.push(x + 1);
                        test_temp.push(y);
                    }
                }
                if (x - 1 >= 0) {
                    neibor = Point(x - 1, y);//左
                    if (y + 1 <= mask.rows && y - 1 >= 0) {
                        neibor_check1 = Point(x - 1, y + 1);//左下
                        neibor_check2 = Point(x - 1, y - 1);//左上
                        if (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255)
                        {
                            check = 1;
                        }
                        else check = 0;
                    }
                    if ((mask.at<uchar>(neibor) == 0) && ((x - 1) < mask.cols) && y <= mask.rows && check == 0)
                    {
                        test.push(x - 1);
                        test.push(y);
                        test_temp.push(x - 1);
                        test_temp.push(y);
                    }
                    else {
                        test_temp.push(x - 1);
                        test_temp.push(y);
                    }
                }
                if (y + 1 <= mask.rows) {
                    neibor = Point(x, y + 1);//下
                    if (x + 1 <= mask.cols && x - 1 >= 0) {
                        neibor_check1 = Point(x - 1, y + 1);//下左
                        neibor_check2 = Point(x + 1, y + 1);//下右
                        neibor_check3 = Point(x + 1, y);//右
                        if ((mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255) || (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check3) == 255))
                        {
                            check = 1;
                        }
                        else check = 0;


                    }

                    if ((mask.at<uchar>(neibor) == 0) && (x < mask.cols) && (y + 1) >= 0 && check == 0)
                    {

                        test.push(x);
                        test.push(y + 1);
                        test_temp.push(x);
                        test_temp.push(y + 1);
                    }
                    else {
                        test_temp.push(x);
                        test_temp.push(y + 1);
                    }
                }
                if (y - 1 >= 0) {
                    neibor = Point(x, y - 1);//上
                    if (x + 1 <= mask.cols && x - 1 >= 0) {
                        neibor_check1 = Point(x - 1, y - 1);//上左
                        neibor_check2 = Point(x + 1, y - 1);//上右					
                        neibor_check3 = Point(x - 1, y);//左
                        if ((mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255) || (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check3) == 255) || (mask.at<uchar>(neibor_check2) == 255 && mask.at<uchar>(neibor_check3) == 255))
                        {
                            check = 1;
                        }
                        else check = 0;
                    }
                    if ((mask.at<uchar>(neibor) == 0) && (x < mask.cols) && (y - 1) <= mask.rows && check == 0)
                    {
                        test.push(x);
                        test.push(y - 1);
                        test_temp.push(x);
                        test_temp.push(y - 1);
                    }
                    else {
                        test_temp.push(x);
                        test_temp.push(y - 1);
                    }
                }
            }

        }
        //imwrite("mask.jpg",mask);
        //fillHole(mask, mask);
        //imwrite("mask1.jpg", mask);
        Mat mask_2 = Mat::zeros(mask.rows, mask.cols, CV_8UC1);

        while (!test_temp.empty()) {
            int temp_y = test_temp.top();
            test_temp.pop();
            int temp_x = test_temp.top();
            test_temp.pop();
            Point temp(temp_x, temp_y);
            mask_2.at<uchar>(temp) = 255;
            fillHole(mask_2, mask_2);
            imwrite("mask1.jpg", mask_2);
            for (int i = 0; i < input.cols; i++) {
                for (int j = 0; j < input.rows; j++) {
                    if (mask_2.at<uchar>(Point(i, j)) == (uchar)255)
                        input.at<Vec3b>(Point(i, j)) = (255, 255, 255);
                        
                }
            }
        }

        //fillHole(input, input);
       /* Mat floodfill;
        floodfill = input_copy.clone();

        floodFill(floodfill, seed, Scalar(128, 128, 128), &ROI_rect, Scalar(7, 7, 7), Scalar(7, 7, 7), 4);

        Mat object = Mat::zeros(floodfill.rows, floodfill.cols, CV_8UC1);
        for (int i = 0; i < floodfill.cols; i++) {
            for (int j = 0; j < floodfill.rows; j++) {
                Point pos = Point(i, j);
                if (floodfill.at<uchar>(pos) == 128)
                    object.at<uchar>(pos) = 255;
            }
        }

        return object;*/
        test = test_copy;
        return input;
    }
    else if (superscaler == 2) {
        Point a;
        //stack<int> test;    // 堆疊
        //test.push(516);//x
        //test.push(105);//y
        //test.push(202);//x
        //test.push(61);//y
        //test.push(195);//x
        //test.push(82);//y
        //test.push(199);//x
        //test.push(71);//y
        stack<int> test_copy;
        test_copy = test;
        Mat mask_copy;
        mask.copyTo(mask_copy);
        Point neibor(1, 2);
        stack<int> test_temp;
        Point neibor_check1;
        Point neibor_check2;
        Point neibor_check3;
        test_temp = test;
        int see_x = mask.cols;
        int see_y = mask.rows;
        int check = 0;
        while (!test.empty())
        {
            a.y = test.top();
            test.pop();
            a.x = test.top();
            test.pop();
            int x = a.x;
            int y = a.y;
            if (mask.at<uchar>(a) != 255) {
                mask.at<uchar>(a) = 255;
                check = 0;

                if (x + 1 >= 0) {
                    neibor = Point(x + 1, y);//右
                    if (y + 1 <= mask.rows && y - 1 >= 0) {
                        neibor_check1 = Point(x + 1, y + 1);//右下
                        neibor_check2 = Point(x + 1, y - 1);//右上
                        if (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255)
                        {
                            check = 1;
                        }
                        else check = 0;
                    }
                    if ((mask.at<uchar>(neibor) == 0) && ((x + 1) < mask.cols) && y <= mask.rows && check == 0)
                    {
                        test.push(x + 1);
                        test.push(y);
                        test_temp.push(x + 1);
                        test_temp.push(y);
                    }
                    else {
                        test_temp.push(x + 1);
                        test_temp.push(y);
                    }
                }
                if (x - 1 >= 0) {
                    neibor = Point(x - 1, y);//左
                    if (y + 1 <= mask.rows && y - 1 >= 0) {
                        neibor_check1 = Point(x - 1, y + 1);//左下
                        neibor_check2 = Point(x - 1, y - 1);//左上
                        if (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255)
                        {
                            check = 1;
                        }
                        else check = 0;
                    }
                    if ((mask.at<uchar>(neibor) == 0) && ((x - 1) < mask.cols) && y <= mask.rows && check == 0)
                    {
                        test.push(x - 1);
                        test.push(y);
                        test_temp.push(x - 1);
                        test_temp.push(y);
                    }
                    else {
                        test_temp.push(x - 1);
                        test_temp.push(y);
                    }
                }
                if (y + 1 <= mask.rows) {
                    neibor = Point(x, y + 1);//下
                    if (x + 1 <= mask.cols && x - 1 >= 0) {
                        neibor_check1 = Point(x - 1, y + 1);//下左
                        neibor_check2 = Point(x + 1, y + 1);//下右
                        neibor_check3 = Point(x + 1, y);//右
                        if ((mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255) || (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check3) == 255))
                        {
                            check = 1;
                        }
                        else check = 0;


                    }

                    if ((mask.at<uchar>(neibor) == 0) && (x < mask.cols) && (y + 1) >= 0 && check == 0)
                    {

                        test.push(x);
                        test.push(y + 1);
                        test_temp.push(x);
                        test_temp.push(y + 1);
                    }
                    else {
                        test_temp.push(x);
                        test_temp.push(y + 1);
                    }
                }
                if (y - 1 >= 0) {
                    neibor = Point(x, y - 1);//上
                    if (x + 1 <= mask.cols && x - 1 >= 0) {
                        neibor_check1 = Point(x - 1, y - 1);//上左
                        neibor_check2 = Point(x + 1, y - 1);//上右					
                        neibor_check3 = Point(x - 1, y);//左
                        if ((mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check2) == 255) || (mask.at<uchar>(neibor_check1) == 255 && mask.at<uchar>(neibor_check3) == 255) || (mask.at<uchar>(neibor_check2) == 255 && mask.at<uchar>(neibor_check3) == 255))
                        {
                            check = 1;
                        }
                        else check = 0;
                    }
                    if ((mask.at<uchar>(neibor) == 0) && (x < mask.cols) && (y - 1) <= mask.rows && check == 0)
                    {
                        test.push(x);
                        test.push(y - 1);
                        test_temp.push(x);
                        test_temp.push(y - 1);
                    }
                    else {
                        test_temp.push(x);
                        test_temp.push(y - 1);
                    }
                }
            }

        }
        Mat mask_2 = Mat::zeros(mask.rows, mask.cols, CV_8UC1);;

        

        while (!test_temp.empty()) {
            int temp_y = test_temp.top();
            test_temp.pop();
            int temp_x = test_temp.top();
            test_temp.pop();
            Point temp(temp_x, temp_y);
            mask_2.at<uchar>(temp) = 255;
            fillHole(mask_2, mask_2);
            
        }
        //imwrite("object.jpg", mask_2);
        /* Mat floodfill;
         floodfill = input_copy.clone();

         floodFill(floodfill, seed, Scalar(128, 128, 128), &ROI_rect, Scalar(7, 7, 7), Scalar(7, 7, 7), 4);

         Mat object = Mat::zeros(floodfill.rows, floodfill.cols, CV_8UC1);
         for (int i = 0; i < floodfill.cols; i++) {
             for (int j = 0; j < floodfill.rows; j++) {
                 Point pos = Point(i, j);
                 if (floodfill.at<uchar>(pos) == 128)
                     object.at<uchar>(pos) = 255;
             }
         }

         return object;*/
         //fillHole(mask_2, mask_2);
        test = test_copy;
        return mask_2;
    }
    
    else { test = test_copy; return input; }



}

#include "adipfinal.h"
#include <windows.h>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ximgproc.hpp>

typedef struct movestep {
    int x;
    int y;
};

Mat Trace(Mat input, Point seed, Rect& ROI_rect);
int rect_dilate(Mat input, Rect& ROI_rect);
Mat Background(Mat input, Mat object);
int Savevideo(char filename[], Mat background, Mat ROI, Mat mask, Rect ROI_rect, movestep move);
int Videoplayer(Mat input, stack <int> seedStack, int globalX_move, int globalY_move,int frame,char c);
void fillHole(const cv::Mat srcimage, cv::Mat& dstimage);
Mat segment(int pointnum,Point bound[], Mat input);
Mat block(Mat background, Mat mask, Point start[], Point end[], int size);
Mat SLIC_trace(Mat input, stack<int> test, Rect& ROI_rect, int superscaler=0);
Mat SLIC_trace1(Mat input, Mat inuptslic, Mat& slic_black,stack<int> test, Rect& ROI_rect, int superscaler = 0);
void test_code();
Mat dynamic(Mat background, Mat seg, int size, movestep* dynamic_dist);
int Videoplayer2(Mat input, stack <int> seedStack, stack <int> backgroundStack, stack<int> backgroundmoveStack, int globalX_move, int globalY_move, int frame, char* fileName,char c = 10);
int Savevideo2(Mat input, stack <int> seedStack, stack <int> backgroundStack, stack<int> backgroundmoveStack, int globalX_move, int globalY_move, int frame, char c = 10);

int Videoplayer3(Mat input, Mat inputslic, Mat slic_black,stack <int> seedStack, stack <int> backgroundStack, stack<int> backgroundmoveStack, int globalX_move, int globalY_move, int frame, char* fileName, char c);
int Savevideo3(Mat input, stack <int> seedStack, stack <int> backgroundStack, stack<int> backgroundmoveStack, int globalX_move, int globalY_move, int frame, char c);
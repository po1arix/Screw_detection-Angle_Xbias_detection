#ifndef ANGLEWITHXBIAS_H
#define ANGLEWITHXBIAS_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <map>



using namespace std;
using namespace cv;

struct ROI {
    int x, y, width, height;
};

ROI get_roi0(const int layer);

double get_pixelpermm(const std::string& layer);

ROI cal_rect_roi(const cv::Point2f& left_top_point, const cv::Point2f& right_bottom_point);

bool in_triangle(const cv::Point2f& point, const cv::Point2f& A, const cv::Point2f& B, const cv::Point2f& C);

bool roi_to_delete(const cv::Point2f& point, const ROI& roi0, const int layer);

bool roi_to_save(const cv::Point2f& point, const ROI& roi0, const int layer);

double processImage(Mat standardImage,Mat inputImage,const int layer, const int detectMode);

# endif

// 模块三 检测零件 偏移角度 以及 横向偏移量

/* 由于机械误差需要反复调用本模块，
 * 先将detect mode设置为0，反复调用本模块检测角度，直到角度误差在容忍范围内，
 * 然后将detect mode设置为1，反复调用本模块检测x偏移量，直到水平偏移量在容忍范围内
 * 容忍范围请自行设定
 */
// 调用本模块 只需调用以下所示主接口
// 主接口 double processImage(Mat standardImage,Mat inputImage, const int label, const int detectMode)

// 接口参数
// standardImage 标准图像的矩阵 和layer相对应，例如layer为1，standardImage为1号零件的横向偏移量为0且角度偏移量为0时的图像
// inputImage 输入图像的矩阵  label 为零件的层数序号(1-5,0是旧版2号零件可用参数，目前5号零件数据缺失)，detectMode 为检测模式，
// detectMode  0 检测角度  1 检测x偏移量

// 接口返回值
// 返回值随detectMode变化，double类型， 角度单位为度 偏移量单位为mm
/* 试用案例
#include "AngleWithXbias.h"
#include "AngleWithXbias.cpp"

int main() {
    cv::Mat inputImage = cv::imread("E:\\ImProcSoftware\\Screw_Dataset\\1016_all\\pianyi\\4_local_3_angle_0.jpg");
    cv::Mat standardImage = cv::imread("E:\\ImProcSoftware\\Screw_Dataset\\1016_all\\pianyi\\4_local_0_angle_0.jpg");
    // 可以读入彩色图像，程序内部会自动转换成灰度图
    // 若手动转换成灰度图，请勿使用imread的cv::IMREAD_GRAYSCALE参数，这样可能导致该模块精度极大下降
    // 请使用cv::cvtColor(inputImage, inputImage, cv::COLOR_BGR2GRAY);
    // cv::cvtColor(inputImage, inputImage, cv::COLOR_BGR2GRAY);
    // cv::cvtColor(standardImage, standardImage, cv::COLOR_BGR2GRAY);
    double angle = processImage(standardImage,inputImage, 4, 0);
    std::cout << "angle: " << angle << std::endl;
    double x = processImage(standardImage,inputImage, 4, 1);
    std::cout << "x: " << x << std::endl;
    return 0;
}

*/
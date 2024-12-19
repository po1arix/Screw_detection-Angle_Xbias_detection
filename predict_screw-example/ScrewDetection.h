#ifndef SCREWDETECTION_H
#define SCREWDETECTION_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
using namespace std;
using namespace cv;

struct Detection {
    Rect box;
    int class_id;
    float confidence;
    string class_name;
};

vector<Detection> processDetections(Mat& output, int image_width, int image_height, float conf_threshold, int cv_width, int cv_height);
void nms(vector<Detection>& detections, float iou_threshold = 0.4);
Detection screwInstall(Mat bgrImage, const string& modelPath, const string& outputImgPath = "");
#endif // SCREWDETECTION_H

//模块一 检测螺钉错漏装情况

// 调用本模块，只需调用以下主接口
// Detection screwInstall(Mat bgrImage, const string& modelPath, const string& outputImgPath = "");

// 接口参数
/* bgrImage 输入图像的矩阵
 * modelPath 模型路径
 * outputImgPath 输出图像的路径，不传参数则不保存输出图像
 */
/*
 struct Detection {
    Rect box;检测框的位置，x,y,width,height
    int class_id;类别id
    float confidence;置信度
    string class_name; 检测结果（本模块主要的输出量）
};
 */

/* 调用示例
#include "ScrewDetection.h"
#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    string imagePath = "../test/zt_12-3-2.jpg";
    cv::Mat inputImg = cv::imread(imagePath, cv::IMREAD_COLOR);
    string modelPath = "../m3_best.onnx";

//保存输出图像
    Detection closest_detection = screwInstall(inputImg, modelPath, "../output.jpg");

//不保存输出图像
//  Detection closest_detection = screwInstall(inputImg, modelPath);

// 此处根据需求读取结果
int result_id = closest_detection.class_id;
string result_name = closest_detection.class_name;

// name 与id的对应关系
//"c" 0 沉头螺钉漏装
//"z" 1 柱头螺钉漏装
//"cic" 2 沉头螺钉正确安装
//"ciz" 3 沉头螺钉错装在柱头螺孔中
//"zic" 4 柱头螺钉错装在沉头螺孔中
//"ziz" 5 柱头螺钉正确安装
    return 0;
}
 */


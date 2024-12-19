#include "ScrewDetection.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp> // 包含OpenCV库

namespace fs = std::filesystem;


// 获取目标文件夹中的所有图片文件路径
std::vector<std::string> getImageFiles(const std::string& folderPath) {
    std::vector<std::string> imageFiles;

    // 遍历文件夹中的所有文件
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            // 检查文件扩展名是否为图片格式（例如：.jpg, .png, .bmp）
            if (extension == ".jpg" || extension == ".png" || extension == ".bmp") {
                imageFiles.push_back(entry.path().string());
            }
        }
    }

    return imageFiles;
}

int main() {


// name 与id的对应关系
//"c" 0 沉头螺钉漏装
//"z" 1 柱头螺钉漏装
//"cic" 2 沉头螺钉正确安装
//"ciz" 3 沉头螺钉错装在柱头螺孔中
//"zic" 4 柱头螺钉错装在沉头螺孔中
//"ziz" 5 柱头螺钉正确安装
    string modelPath = "../m2_best.onnx";
    Detection closest_detection;
    int result_id;
    std::string folderPath = "/Users/yxhu/Desktop/screw/m2_dataset/images/val"; // 替换为你的图片文件夹路径

    // 打开文件，准备写入结果
    std::ofstream outfile;
    outfile.open("/Users/yxhu/Desktop/screw/result.txt", std::ios::app);

    // 获取图片文件列表
    std::vector<std::string> imageFiles = getImageFiles(folderPath);

    // 遍历图片文件路径，使用 cv::imread 读取图片
    for (const auto& filePath : imageFiles) {
        // 使用 cv::imread 读取图片
        cv::Mat image = cv::imread(filePath);
        Detection closest_detection = screwInstall(image, modelPath);
        result_id = closest_detection.class_id;
        outfile << filePath << " " << result_id << std::endl;
        // 检查图片是否成功读取
        if (image.empty()) {
            std::cerr << "Failed to read image: " << filePath << std::endl;
            continue;
        }
    }
    return 0 ;
}


#include "AngleWithXbias.hpp"
#include <iostream>
#include <filesystem>
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
    int layer = 2;
//    std::string folderPath = "/Users/yxhu/Desktop/orb/pianyi_split/" + std::to_string(layer); // 替换为你的图片文件夹路径
    std::string folderPath = "/Users/yxhu/Desktop/orb/pianyi_split/2new"; // 替换为你的图片文件夹路径
    std::string standardPath = "/Users/yxhu/Desktop/orb/pianyi_split/standard_image";
    double angle;
    double x_bias;
    // 打开文件，准备写入结果
    std::ofstream outfile;
    outfile.open("/Users/yxhu/Desktop/orb/new_orb/result.txt", std::ios::app);

    // 获取图片文件列表
    std::vector<std::string> imageFiles = getImageFiles(folderPath);

    // 遍历图片文件路径，使用 cv::imread 读取图片
    for (const auto& filePath : imageFiles) {
        // 使用 cv::imread 读取图片
        cv::Mat image = cv::imread(filePath);
        cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

//        cv::Mat standardImage = cv::imread(standardPath + "/" + std::to_string(layer) + ".jpg");
        cv::Mat standardImage = cv::imread(standardPath + "/2new.jpg");
        angle = processImage(standardImage, image, layer, 0);
        x_bias = processImage(standardImage, image, layer, 1);
        // 检查图片是否成功读取
        if (image.empty()) {
            std::cerr << "Failed to read image: " << filePath << std::endl;
            continue;
        }

        // 输出图片的尺寸信息
//        std::cout << "Successfully read image: " << filePath << std::endl;

//        std::cout << "example_angle: " << angle << std::endl;
//        std::cout << "example_xbias: " << x_bias << std::endl;
        outfile << filePath << " " << angle << " " << x_bias << std::endl;

    }

    return 0;
}
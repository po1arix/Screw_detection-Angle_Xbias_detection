# include "ScrewDetection.h"

int main() {
    // 待检测图像初始化
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

    cout << "result_id: " << result_id << " result_name: " << result_name << endl;
    return 0;
}



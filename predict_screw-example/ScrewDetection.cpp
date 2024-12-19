#include "ScrewDetection.h"

using namespace std;
using namespace cv;



vector<Detection> processDetections(Mat& output, int image_width, int image_height, float conf_threshold, int cv_width, int cv_height) {
    vector<Detection> detections;

    for (int i = 0; i < output.rows; i++) {
        float* data = (float*)output.ptr(i);

        float confidence = data[4];
        if (confidence < conf_threshold) continue;

        float max_class_score = -1;
        int class_id = -1;
        for (int j = 5; j < 11; j++) {
            if (data[j] > max_class_score) {
                max_class_score = data[j];
                class_id = j - 5;
            }
        }
        if (max_class_score * confidence > conf_threshold) {
            float x = data[0] * image_width / cv_width;
            float y = data[1] * image_height / cv_height;
            float w = data[2] * image_width / cv_width;
            float h = data[3] * image_height / cv_height;
            int left = int(x - w / 2);
            int top = int(y - h / 2);
            int width = int(w);
            int height = int(h);

            detections.push_back({Rect(left, top, width, height), class_id, confidence * max_class_score});
        }
    }
    return detections;
}

void nms(vector<Detection>& detections, float iou_threshold) {
    std::sort(detections.begin(), detections.end(), [](const Detection& a, const Detection& b) {
        return a.confidence > b.confidence;
    });

    vector<bool> suppress(detections.size(), false);

    for (size_t i = 0; i < detections.size(); ++i) {
        if (suppress[i]) continue;

        Rect box1 = detections[i].box;
        for (size_t j = i + 1; j < detections.size(); ++j) {
            if (suppress[j]) continue;

            Rect box2 = detections[j].box;
            float iou = (box1 & box2).area() / float(box1.area() + box2.area() - (box1 & box2).area());

            if (iou > iou_threshold) {
                suppress[j] = true;
            }
        }
    }

    detections.erase(std::remove_if(detections.begin(), detections.end(), [&](const Detection& det) {
        return suppress[&det - &detections[0]];
    }), detections.end());
}

Detection drawDetections(Mat& image, const vector<Detection>& detections, const vector<string>& class_names) {
    Detection closest_detection;
    closest_detection.confidence = -1.0f;

    if (detections.empty()) {
        cout << "No detections available to draw." << endl;
        return closest_detection;
    }

    Point image_center(image.cols / 2, image.rows / 2);
    float min_distance = FLT_MAX;

    for (const auto& detection : detections) {
        Point box_center(
                detection.box.x + detection.box.width / 2,
                detection.box.y + detection.box.height / 2
        );

        float distance = norm(box_center - image_center);

        if (distance < min_distance) {
            min_distance = distance;
            closest_detection = detection;
        }
    }

    rectangle(image, closest_detection.box, Scalar(0, 255, 0), 2);
    string label = class_names[closest_detection.class_id] + ": " + to_string(closest_detection.confidence);
    closest_detection.class_name = class_names[closest_detection.class_id];
    putText(image, label, Point(closest_detection.box.x, closest_detection.box.y - 5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);

    return closest_detection;
}

Detection screwInstall(Mat bgrImage, const string& modelPath, const string& outputImgPath) {
    vector<string> class_names = {"c", "z", "cic", "ciz", "zic", "ziz"};
    float conf_threshold = 0.5;
    if (bgrImage.empty()) {
        cout << "Image not found!" << endl;
        return {};
    }

    Size originalSize(bgrImage.cols, bgrImage.rows);

    dnn::Net net = dnn::readNetFromONNX(modelPath);
    int cv_width = 640;
    int cv_height = 640;
    Mat blob = dnn::blobFromImage(bgrImage, 1.0 / 255.0, Size(cv_width, cv_height), Scalar(), true);
    net.setInput(blob);
    vector<Mat> output_probs;
    vector<String> output_layer_names = net.getUnconnectedOutLayersNames();
    net.forward(output_probs, output_layer_names);

    Mat prediction = output_probs[0];
    prediction = prediction.reshape(1, prediction.size[1]);

    vector<Detection> detections = processDetections(prediction, originalSize.width, originalSize.height, conf_threshold, cv_width, cv_height);
    nms(detections);

    Detection closest_detection = drawDetections(bgrImage, detections, class_names);

    if (!outputImgPath.empty()) {
        imwrite(outputImgPath, bgrImage);
    }

    return closest_detection;
}


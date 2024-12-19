#include "AngleWithXbias.hpp"




ROI get_roi0(const int layer) {
    if (layer == 1) {
        return {1684, 1300, 500, 900};
    } else if (layer == 2) {
        return {1884, 1590, 322, 260};
    } else if (layer == 3) {
        return {1750, 1320, 600, 600};
    } else if (layer == 4) {
        return {1680, 1250, 720, 650};
    } else if (layer == 5) {
        return {1716, 1185, 703, 669};
    }
    return {0, 0, 0, 0};
}


double get_pixelpermm(const std::string& layer) {
    if (layer == "1") {
        return 122.0 / 30.0;
    } else if (layer == "2") {
        return 112.0 / 30.0;
    } else if (layer == "3") {
        return 104.0 / 30.0;
    } else if (layer == "4") {
        return 104.0 / 30.0;
    } else if (layer == "5") {
        return 100.0 / 30.0;
    }
    return 0.0;
}

ROI cal_rect_roi(const cv::Point2f& left_top_point, const cv::Point2f& right_bottom_point) {
    return {static_cast<int>(left_top_point.x), static_cast<int>(left_top_point.y),
            static_cast<int>(right_bottom_point.x - left_top_point.x),
            static_cast<int>(right_bottom_point.y - left_top_point.y)};
}


bool in_triangle(const cv::Point2f& point, const cv::Point2f& A, const cv::Point2f& B, const cv::Point2f& C) {
    // 判断点是否在三角形内
    cv::Point2f v0 = C - A;
    cv::Point2f v1 = B - A;
    cv::Point2f v2 = point - A;

    float dot00 = v0.dot(v0);
    float dot01 = v0.dot(v1);
    float dot02 = v0.dot(v2);
    float dot11 = v1.dot(v1);
    float dot12 = v1.dot(v2);

    float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
    if (u < 0 || u > 1) {
        return false;
    }
    float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
    if (v < 0 || v > 1) {
        return false;
    }
    return u + v <= 1;
}

bool roi_to_save(const cv::Point2f& point, const ROI& roi0, const int layer) {
    // 保留roi中的点

    cv::Point2f point_in_roi0(point.x + roi0.x, point.y + roi0.y);
    if (layer == 2) {
        // 点是否在roi1或roi2中
//        cv::Rect roi1(2028, 1580, 56, 260);
//        cv::Rect roi2(1884, 1590, 322, 260);
        cv::Point2f left_top_point1(2028, 1580);
        cv::Point2f right_bottom_point1(2066, 1843);
        cv::Point2f left_top_point2(1905,1688);
        cv::Point2f right_bottom_point2(2162, 1749);

        ROI roi_1_ori = cal_rect_roi(left_top_point1, right_bottom_point1);
        cv::Rect roi1(roi_1_ori.x, roi_1_ori.y, roi_1_ori.width, roi_1_ori.height);
        ROI roi_2_ori = cal_rect_roi(left_top_point2, right_bottom_point2);
        cv::Rect roi2(roi_2_ori.x, roi_2_ori.y, roi_2_ori.width, roi_2_ori.height);

        return roi1.contains(point_in_roi0) || roi2.contains(point_in_roi0);
    }
}

bool roi_to_delete(const cv::Point2f& point, const ROI& roi0, const int layer) {
    // 删除roi中的点
    cv::Point2f point_in_roi0(point.x + roi0.x, point.y + roi0.y);
    if (layer == 1) {
        bool tri1 = in_triangle(point_in_roi0, cv::Point2f(roi0.x, roi0.y), cv::Point2f(1786, roi0.y), cv::Point2f(roi0.x, 1545));
        bool tri2 = in_triangle(point_in_roi0, cv::Point2f(2305, roi0.y), cv::Point2f(roi0.x+roi0.width, roi0.y), cv::Point2f(roi0.x+roi0.width, 1584));
        bool tri3 = in_triangle(point_in_roi0, cv::Point2f(roi0.x, 1969), cv::Point2f(roi0.x, roi0.y+roi0.height), cv::Point2f(1744, roi0.y+roi0.height));
        bool tri4 = in_triangle(point_in_roi0, cv::Point2f(2340, roi0.y+roi0.height), cv::Point2f(roi0.x+roi0.width, roi0.y+roi0.height), cv::Point2f(roi0.x+roi0.width, 2004));
        cv::Point2f left_top_point(1865, 1894);
        cv::Point2f right_bottom_point(2099, 2212);
        ROI roi_1_ori = cal_rect_roi(left_top_point,right_bottom_point);
        cv::Rect roi1(roi_1_ori.x, roi_1_ori.y, roi_1_ori.width, roi_1_ori.height);
        return !roi1.contains(point_in_roi0) && !tri1 && !tri2 && !tri3 && !tri4;
    }
}


double processImage(Mat standardImage,Mat inputImage , const int layer, const int detectMode) {
    // 检查输入图像是否为空
    if (standardImage.empty() || inputImage.empty()) {
        cout << "invalid image" << endl;
        return 0;
    }
    // 检查输入图像是否是灰度图,不是则转换成灰度图
    if (standardImage.channels() != 1) {
        cv::cvtColor(standardImage, standardImage, cv::COLOR_BGR2GRAY);
    }
    if (inputImage.channels() != 1) {
        cv::cvtColor(inputImage, inputImage, cv::COLOR_BGR2GRAY);
    }


    // 框定初始检测区域
    ROI roi0 = get_roi0(layer);
    Mat img1_roi = standardImage(Rect(roi0.x, roi0.y, roi0.width, roi0.height));
    Mat img2_roi = inputImage(Rect(roi0.x, roi0.y, roi0.width, roi0.height));

    // 进行角点检测
    cv::Ptr<cv::ORB> orb = cv::ORB::create(1000);
    std::vector<cv::KeyPoint> keypoints1, keypoints2, filtered_keypoints1;
    cv::Mat descriptors1, descriptors2, filtered_descriptors1;
    orb->detectAndCompute(img1_roi, cv::noArray(), keypoints1, descriptors1);
    orb->detectAndCompute(img2_roi, cv::noArray(), keypoints2, descriptors2);


    // 初始化 matcher,以及匹配点集
    cv::BFMatcher matcher(cv::NORM_HAMMING, true);
    std::vector<cv::DMatch> matches;
    std::vector<cv::Point2f> src_pts, dst_pts;
    if (layer == 5 || layer == 4 || layer == 3) {
        // 进行特征点匹配
        matcher.match(descriptors1, descriptors2, matches);
        // 可视化  非必须
//        cv::Mat img_matches;
//        std::vector<cv::KeyPoint> keypoints1_global, keypoints2_global;
//        for (const auto& keypoint : keypoints1) {
//            keypoints1_global.push_back(cv::KeyPoint(keypoint.pt + cv::Point2f(roi0.x, roi0.y), keypoint.size));
//        }
//        for (const auto& keypoint : keypoints2) {
//            keypoints2_global.push_back(cv::KeyPoint(keypoint.pt + cv::Point2f(roi0.x, roi0.y), keypoint.size));
//        }
//        cv::drawMatches(standardImage, keypoints1_global, inputImage, keypoints2_global, matches, img_matches,
//                        cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
//                        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
//        cv::imwrite("../matches.jpg", img_matches);
        // 点集读取
        for (const auto &match: matches) {
            src_pts.push_back(keypoints1[match.queryIdx].pt);
            dst_pts.push_back(keypoints2[match.trainIdx].pt);
        }
    }

    if (layer == 2) {
        // 对标准图像中的角点进行筛选
        for (const auto &keypoint: keypoints1) {
            if (roi_to_save(keypoint.pt, roi0, layer)) {
                filtered_keypoints1.push_back(keypoint);
            }
        }
        // 重新计算描述子
        orb->compute(img1_roi, filtered_keypoints1, filtered_descriptors1);
        // 进行特征点匹配
        matcher.match(filtered_descriptors1, descriptors2, matches);
        // 可视化  非必须
        cv::Mat img_matches;
        std::vector<cv::KeyPoint> keypoints1_global, keypoints2_global;
        for (const auto& keypoint : filtered_keypoints1) {
            keypoints1_global.push_back(cv::KeyPoint(keypoint.pt + cv::Point2f(roi0.x, roi0.y), keypoint.size));
        }
        for (const auto& keypoint : keypoints2) {
            keypoints2_global.push_back(cv::KeyPoint(keypoint.pt + cv::Point2f(roi0.x, roi0.y), keypoint.size));
        }
        cv::drawMatches(standardImage, keypoints1_global, inputImage, keypoints2_global, matches, img_matches,
                        cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
                        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        cv::imwrite("../matches.jpg", img_matches);
        // 点集读取
        for (const auto &match: matches) {
            src_pts.push_back(filtered_keypoints1[match.queryIdx].pt);
            dst_pts.push_back(keypoints2[match.trainIdx].pt);
        }
    }

    if (layer == 1) {
        //对标准图像中的角点进行筛选
        for (const auto &keypoint: keypoints1) {
            if (roi_to_delete(keypoint.pt, roi0, layer)) {
                filtered_keypoints1.push_back(keypoint);
            }
        }

        // 重新计算描述子
        orb->compute(img1_roi, filtered_keypoints1, filtered_descriptors1);
        // 进行特征点匹配
        matcher.match(descriptors1, descriptors2, matches);
        std::vector<cv::DMatch> filtered_matches;

        // 过滤无效的匹配项
        for (const auto& match : matches) {
            if (match.queryIdx >= 0 && match.queryIdx < static_cast<int>(filtered_keypoints1.size()) &&
                match.trainIdx >= 0 && match.trainIdx < static_cast<int>(keypoints2.size())) {
                filtered_matches.push_back(match);
            }
        }
        // 可视化  非必须
//        cv::Mat img_matches;
//        std::vector<cv::KeyPoint> keypoints1_global, keypoints2_global;
//        for (const auto& keypoint : filtered_keypoints1) {
//            keypoints1_global.push_back(cv::KeyPoint(keypoint.pt + cv::Point2f(roi0.x, roi0.y), keypoint.size));
//        }
//        for (const auto& keypoint : keypoints2) {
//            keypoints2_global.push_back(cv::KeyPoint(keypoint.pt + cv::Point2f(roi0.x, roi0.y), keypoint.size));
//        }
//        cv::drawMatches(standardImage, keypoints1_global, inputImage, keypoints2_global, filtered_matches, img_matches,
//                        cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
//                        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
//        cv::imwrite("../matches.jpg", img_matches);
        // 点集读取
        for (const auto &match: matches) {
            src_pts.push_back(keypoints1[match.queryIdx].pt);
            dst_pts.push_back(keypoints2[match.trainIdx].pt);
        }
    }

    // 计算仿射矩阵
    cv::Mat M = cv::estimateAffinePartial2D(dst_pts, src_pts, cv::noArray(), cv::RANSAC, 3.0);

    // 进一步计算偏移量
    if (!M.empty()) {
        double cos_theta = M.at<double>(0, 0);
        double sin_theta = M.at<double>(1, 0);
        if (detectMode == 0) return -std::atan2(sin_theta, cos_theta) * 180.0 / CV_PI;
        if (detectMode == 1) return -M.at<double>(0, 2) / get_pixelpermm(std::to_string(layer));
    }
}

//
//int main() {
//    std::string img1_path = "../pianyi_split/4/4_local_0_angle_0.jpg";
//    std::string img2_path = "../pianyi_split/4/4_local_-1.5_angle_-2.5.jpg";
//    std::string img3_path = "../pianyi_split/4/4_local_3_angle_0.jpg";
//    Mat img1 = cv::imread(img1_path);
//    Mat img2 = cv::imread(img2_path);
//    Mat img3 = cv::imread(img3_path);
//
//    double angle = processImage(img1, img2, 4, 0);
//    double x_bias = processImage(img1, img3, 4, 1);
//    cout << "example_angle: " << angle << endl;
//    cout << "example_xbias: " << x_bias << endl;
//    return 0;
//}
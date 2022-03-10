#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <contour.h>
#include <vector>

void displayContour(const cv::Mat &img, const Contour &contour) {
    auto points = contour.getPoints();

    for (int i = 0; i < points.size() - 1; i++) {
        if (i == 0) {
            cv::line(img, points[points.size() - 1], points[i], (255, 0, 0));
        }
        cv::line(img, points[i], points[i + 1], (255, 0, 0));
    }
}

int main() {

    cv::namedWindow("main", cv::WINDOW_NORMAL);
    cv::Mat img = cv::imread("coin.jpeg");

    cv::Size s = img.size();
    int offset = 10;
    std::vector<cv::Point> points{cv::Point(offset, offset),
                                  cv::Point(s.width - offset, offset),
                                  cv::Point(s.width - offset, s.height - offset),
                                  cv::Point(offset, s.height - offset)};
    auto contour = Contour(points);

    displayContour(img, contour);

    cv::imshow("main", img);
    cv::waitKey(0);

    cv::destroyAllWindows();

    return 0;
}

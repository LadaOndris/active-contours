#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <contour.h>
#include <vector>
#include <examples/base.h>


void displayContour(const cv::Mat &img, const Contour &contour) {
    auto points = contour.getPoints();

    for (int i = 0; i < points.size(); i++) {
        auto lineColor = cv::Scalar(220, 50, 0);
        auto nextPoint = contour.getPointByIndexWithOverflow(i + 1);
        cv::line(img, points[i], nextPoint, cv::Scalar(220, 50, 0), 2);
    }
    for (int i = 0; i < points.size(); i++) {
        auto pointColor = cv::Scalar(0, 50, 220);
        cv::circle(img, points[i], 4, pointColor, -1);
    }
}

Contour getInitialContour(int offset, const cv::Size imageSize, int numPoints) {
    std::vector<cv::Point> points{cv::Point(offset, offset),
                                  cv::Point(imageSize.width - offset, offset),
                                  cv::Point(imageSize.width - offset, imageSize.height - offset),
                                  cv::Point(offset, imageSize.height - offset)};
    auto contour = Contour(points);
    contour.samplePointsUniformly(numPoints);
    return contour;
}

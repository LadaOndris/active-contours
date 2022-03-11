//
// Created by lada on 3/10/22.
//

#ifndef SNAKES_CONTOUR_H
#define SNAKES_CONTOUR_H

#include <vector>
#include <opencv2/core/types.hpp>

using namespace std;


class Contour {

public:
    explicit Contour(vector<cv::Point>);

    vector<cv::Point> getPoints() const;

    void samplePointsUniformly(int num_points);

    double difference(const Contour &other) const;

    cv::Point getPointByIndexWithOverflow(int index) const;

private:
    double getContourLength() const;

    double pointsDist(cv::Point a, cv::Point b) const;

    double l2norm(cv::Point p) const;

    vector<cv::Point> points;

    double moveToNewUniformPosition(const cv::Point startLinePoint, const cv::Point endLinePoint,
                                    cv::Point &currentLinePoint, double moveDist);

    void removeDuplicatePoints();
};


#endif //SNAKES_CONTOUR_H

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

private:
    vector<cv::Point> points;
};


#endif //SNAKES_CONTOUR_H

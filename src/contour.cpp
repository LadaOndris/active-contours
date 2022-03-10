//
// Created by lada on 3/10/22.
//

#include "contour.h"

Contour::Contour(vector<cv::Point> contour_points) : points(contour_points) {

}

vector<cv::Point> Contour::getPoints() const {
    return points;
}

#ifndef SNAKES_BASE_H
#define SNAKES_BASE_H

void displayContour(const cv::Mat &img, const Contour &contour);

Contour getInitialContour(int offset, const cv::Size imageSize, int numPoints);

#endif //SNAKES_BASE_H

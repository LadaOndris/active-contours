//
// Created by lada on 3/12/22.
//

#ifndef SNAKES_ACTIVECONTOURS_H
#define SNAKES_ACTIVECONTOURS_H

#include <contour.h>

class Snake {
public:
    explicit Snake(Contour initialContour);

    double step(int windowSize, double weightElasticity, double weightSmoothness, int numPoints);

    Contour getContour() const;

    void updateImage(const cv::Mat& img);

    cv::Mat getEnergyImage() const;

private:
    Contour contour;
    cv::Mat energyImage;

    void gaussianMagnitude(const cv::_InputArray &img, const cv::_OutputArray &output);

    void blurredGaussianMagnitudeSquared(const cv::_InputArray &img, const cv::_OutputArray &output);

    void clippedWindow(cv::Point &p1, cv::Point &p2, const cv::Point &centerPoint, int windowSize,
                       const cv::Size imageSize);

    int minMaxClip(int val, int min, int max);

    vector<cv::Point> updateContour(const cv::Mat &img, const Contour &contour, int windowSize);

    double computeElasticity(const cv::Point p1, const cv::Point p2);

    double computeSmoothness(const cv::Point p1, const cv::Point p2, const cv::Point p3);

    void setContourEnergy(cv::Mat &energyContour, const Contour &contour, int windowSize, const cv::Size imageSize,
                          double weightElasticity, double weightSmoothness);

};


#endif //SNAKES_ACTIVECONTOURS_H

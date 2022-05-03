//
// Created by lada on 3/12/22.
//

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <utility>
#include <snake.h>
#include <contour.h>


Snake::Snake(Contour initialContour) : contour(std::move(initialContour)) {
}

double Snake::step(int windowSize, double weightElasticity, double weightSmoothness, int numPoints) {
    // Compute energy
    cv::Mat energyContour;
    cv::Mat energyTotal;
    setContourEnergy(energyContour, contour, windowSize, energyImage.size(), weightElasticity, weightSmoothness);
    cv::add(energyImage, energyContour, energyTotal, cv::noArray(), energyContour.type());

    // Update contour
    auto newPoints = updateContour(energyTotal, contour, windowSize);
    auto newContour = Contour(newPoints);
    if (newContour.getContourLength() < 20) {
        // The contour is too small for further processing. End it here.
        return -1;
    }
    newContour.samplePointsUniformly(numPoints);

    // Compute difference with previous contour
    double contoursDifference = contour.difference(newContour);

    // Update the current contour instance
    contour = newContour;
    return contoursDifference;
}

Contour Snake::getContour() const {
    return contour;
}

void Snake::gaussianMagnitude(cv::InputArray img, cv::OutputArray output) {
    // Apply a cv::GaussianBlur to reduce the noise
    cv::Mat blurred;
    int ksize = 25;
    int sigma = 19;
    cv::GaussianBlur(img, blurred,
                     cv::Size(ksize, ksize),
                     sigma, sigma, cv::BORDER_DEFAULT);

    cv::Mat1b gray;
    //Convert to grayscale
    if (blurred.channels() == 3) {
        cvtColor(blurred, gray, cv::COLOR_BGR2GRAY);
    }
    else {
        gray = blurred;
    }

    //Compute dx and dy derivatives
    int scale = 1;
    int delta = 0;
    int ddepth = CV_32F;
    cv::Mat grad_x, grad_y;
    cv::Mat abs_grad_x, abs_grad_y;

    Sobel(gray, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
    Sobel(gray, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);

    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);

    //magnitude(grad_x, grad_y, output);
    cv::Mat dx_squared, dy_squared;
    cv::pow(abs_grad_x, 2, dx_squared);
    cv::pow(abs_grad_y, 2, dy_squared);

    addWeighted(dx_squared, 0.5, dy_squared, 0.5, 0, output);
}

void Snake::blurredGaussianMagnitudeSquared(cv::InputArray img, cv::OutputArray output) {

    cv::Mat magnitude;
    gaussianMagnitude(img, magnitude);

    // Blur the result
    int ksize = 19;
    int sigma = 19;
    cv::GaussianBlur(magnitude, output,
                     cv::Size(ksize, ksize),
                     sigma, sigma, cv::BORDER_DEFAULT);
}


// Compute energyImage for all points of the image - Blurred Gradient Magnitude Squared
// For each contour point select a new point inside a fixed-sized window with minimal energyImage

int Snake::minMaxClip(int val, int min, int max) {
    if (val < min) {
        return min;
    } else if (val > max) {
        return max;
    }
    return val;
}

void Snake::clippedWindow(cv::Point &p1, cv::Point &p2, const cv::Point &centerPoint,
                   int windowSize, const cv::Size imageSize) {
    auto img_width = imageSize.width;
    auto img_height = imageSize.height;

    int x_start = minMaxClip(centerPoint.x - windowSize, 0, img_width - 1);
    int x_end = minMaxClip(centerPoint.x + windowSize, 0, img_width - 1);

    int y_start = minMaxClip(centerPoint.y - windowSize, 0, img_height - 1);
    int y_end = minMaxClip(centerPoint.y + windowSize, 0, img_height - 1);

    p1.x = x_start;
    p1.y = y_start;
    p2.x = x_end;
    p2.y = y_end;
}

std::vector<cv::Point> Snake::updateContour(const cv::Mat &img, const Contour &contour, int windowSize) {

    std::vector<cv::Point> newPoints;

    for (auto point : contour.getPoints()) {
        // Get image patch
        cv::Point p1;
        cv::Point p2;
        clippedWindow(p1, p2, point, windowSize, img.size());
        cv::Mat patch = img(cv::Range(p1.y, p2.y), cv::Range(p1.x, p2.x));

        // Find minimal energy position inside the patch
        double minVal;
        cv::Point minLoc;
        minMaxLoc(patch, &minVal, nullptr, &minLoc, nullptr);

        // Insert the new point
        int newX = p1.x + minLoc.x;
        int newY = p1.y + minLoc.y;
        newPoints.emplace_back(cv::Point(newX, newY));
    }
    return newPoints;
}

/**
 * Elasticity is computed between two points.
 */
double Snake::computeElasticity(const cv::Point p1, const cv::Point p2) {
    return pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2);
}

/**
 * Elasticity is computed between three points.
 */
double Snake::computeSmoothness(const cv::Point p1, const cv::Point p2, const cv::Point p3) {
    double smoothnessX = pow(p3.x - 2 * p2.x + p1.x, 2);
    double smoothnessY = pow(p3.y - 2 * p2.y + p1.y, 2);
    return smoothnessX + smoothnessY;
}

void Snake::setContourEnergy(cv::Mat &energyContour, const Contour &contour, int windowSize, const cv::Size imageSize,
                      double weightElasticity, double weightSmoothness) {
    energyContour = cv::Mat::zeros(imageSize, CV_64FC1);

    auto points = contour.getPoints();
    for (int i = 0; i < points.size(); i++) {
        auto point = points[i];
        auto pointNext = contour.getPointByIndexWithOverflow(i + 1);
        auto pointPrevious = contour.getPointByIndexWithOverflow(i - 1);
        cv::Point p1;
        cv::Point p2;
        clippedWindow(p1, p2, point, windowSize, energyContour.size());

        for (int y = p1.y; y <= p2.y; y++) {
            for (int x = p1.x; x <= p2.x; x++) {
                // Elasticity
                energyContour.at<double>(y, x) += weightElasticity * computeElasticity(cv::Point(x, y), pointNext);
                // Smoothness
                energyContour.at<double>(y, x) +=
                        weightSmoothness * computeSmoothness(pointPrevious, cv::Point(x, y), pointNext);
            }
        }
    }
}

void Snake::updateImage(const cv::Mat& img) {
    blurredGaussianMagnitudeSquared(img, energyImage);
    // Inverse black and white, so that the energy can be minimized.
    // Edges will be black, and, thus, zero.
    energyImage = 255 - energyImage;
}

cv::Mat Snake::getEnergyImage() const {
    return energyImage;
}


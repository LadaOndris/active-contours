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

void gaussianMagnitude(cv::InputArray img, cv::OutputArray output) {
    // Apply a cv::GaussianBlur to reduce the noise
    cv::Mat blurred;
    cv::GaussianBlur(img, blurred, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);

    //Convert to grayscale
    cv::Mat1b gray;
    cvtColor(blurred, gray, cv::COLOR_BGR2GRAY);

    //Compute dx and dy derivatives
    int scale = 1;
    int delta = 0;
    int ddepth = CV_32F;
    cv::Mat grad_x, grad_y;
    cv::Mat abs_grad_x, abs_grad_y;

//    Sobel(gray, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
//    Sobel(gray, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);

    cv::Scharr(gray, grad_x, ddepth, 1, 0, scale, delta, cv::BORDER_DEFAULT);
    cv::Scharr(gray, grad_y, ddepth, 0, 1, scale, delta, cv::BORDER_DEFAULT);

    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);

    //magnitude(grad_x, grad_y, output);
    cv::Mat dx_squared, dy_squared;
    cv::pow(abs_grad_x, 2, dx_squared);
    cv::pow(abs_grad_y, 2, dy_squared);


    addWeighted(dx_squared, 0.5, dy_squared, 0.5, 0, output);


    double minVal;
    double maxVal;
    cv::Point minLoc;
    cv::Point maxLoc;

    minMaxLoc(grad_x, &minVal, &maxVal, &minLoc, &maxLoc);

    std::cout << minVal << ", " << maxVal  << std::endl;
}

void blurredGaussianMagnitudeSquared(cv::InputArray img, cv::OutputArray output) {

    cv::Mat magnitude;
    gaussianMagnitude(img, magnitude);

    // cv::Mat1f magnitudeSquared;
    // cv::pow(magnitude, 2, output);

    // Blur the result
    int ksize = 19;
    int sigma = 15;
    cv::GaussianBlur(magnitude, output,
                     cv::Size(ksize, ksize),
                     sigma, sigma, cv::BORDER_DEFAULT);
}


// Compute energyImage for all points of the image - Blurred Gradient Magnitude Squared
// For each contour point select a new point inside a fixed-sized window with minimal energyImage

int minMaxClip(int val, int min, int max) {
    if (val < min) {
        return min;
    }
    else if (val > max) {
        return max;
    }
    return val;
}

std::vector<cv::Point> updateContour(const cv::Mat &img, const Contour &contour, int windowSize) {
    auto img_width =  img.size().width;
    auto img_height = img.size().height;
    std::vector<cv::Point> newPoints;

    for (auto point : contour.getPoints()) {
        // Get image patch
        int x_start = minMaxClip(point.x - windowSize, 0, img_width - 1);
        int x_end = minMaxClip(point.x + windowSize, 0, img_width - 1);

        int y_start = minMaxClip(point.y - windowSize, 0, img_height - 1);
        int y_end = minMaxClip(point.y + windowSize, 0, img_height - 1);

        cv::Mat patch = img(cv::Range(y_start, y_end), cv::Range(x_start, x_end));

        // Find minimal energy position inside the patch
        double minVal;
        double maxVal;
        cv::Point minLoc;
        cv::Point maxLoc;

        minMaxLoc(patch, &minVal, &maxVal, &minLoc, &maxLoc);

        int newX = x_start + maxLoc.x;
        int newY = y_start + maxLoc.y;
        newPoints.emplace_back(cv::Point(newX, newY));
    }
    return newPoints;
}



int main() {

    cv::namedWindow("main", cv::WINDOW_NORMAL);
    cv::Mat img = cv::imread("coin.jpeg");

    cv::Size s = img.size();
    int offset = 200;
    std::vector<cv::Point> points{cv::Point(offset, offset),
                                  cv::Point(s.width - offset, offset),
                                  cv::Point(s.width - offset, s.height - offset),
                                  cv::Point(offset, s.height - offset)};
    auto contour = Contour(points);

    cv::Mat imageEnergy;
    blurredGaussianMagnitudeSquared(img, imageEnergy);
    displayContour(imageEnergy, contour);

    auto newPoints = updateContour(imageEnergy, contour, 80);
    auto newContour = Contour(newPoints);
    displayContour(imageEnergy, newContour);

    cv::imshow("main", imageEnergy);

    int ESCAPE_KEY = 27;
    while((cv::waitKey() & 0xEFFFFF) != ESCAPE_KEY);

    cv::destroyAllWindows();

    return 0;
}

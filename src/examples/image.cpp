#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <contour.h>
#include <snake.h>
#include <examples/base.h>


void startImageSnake(const string& imagePath) {
    cv::namedWindow("contour", cv::WINDOW_NORMAL);
    cv::namedWindow("energy", cv::WINDOW_NORMAL);
    cv::Mat image = cv::imread(imagePath);

    cv::Size imageSize = image.size();
    int offset = 50;
    int numPoints = 50;
    int windowSize = 10;
    double weightElasticity = 0.01;
    double weightSmoothness = 0.05;
    int iterations = 200;
    cv::Mat drawing;

    Contour contour = getInitialContour(offset, imageSize, numPoints);
    Snake snake(contour);
    snake.updateImage(image);

    for (int i = 0; i < iterations; i++) {
        if (i == 150) {
            cv::imwrite("saved_examples/image_contour.png", drawing);
            cv::imwrite("saved_examples/image_energy.png", snake.getEnergyImage());
        }

        double contoursDifference = snake.step(windowSize, weightElasticity, weightSmoothness, numPoints);
        cout << contoursDifference << endl;

        drawing = image.clone(); // Clean image
        displayContour(drawing, snake.getContour());

        cv::imshow("contour", drawing);
        cv::imshow("energy", snake.getEnergyImage());
        cv::waitKey(1);
    }

    int ESCAPE_KEY = 27;
    while ((cv::waitKey() & 0xEFFFFF) != ESCAPE_KEY);

    cv::destroyAllWindows();
}
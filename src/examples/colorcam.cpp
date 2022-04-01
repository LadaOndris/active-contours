#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <contour.h>
#include <snake.h>
#include <examples/base.h>

void startWebcamSnake() {
    cv::namedWindow("contour", cv::WINDOW_NORMAL);
    cv::namedWindow("energy", cv::WINDOW_NORMAL);
    cv::Mat image;

    cv::VideoCapture cap(0);
    cap >> image;

    cv::Size imageSize = image.size();
    int offset = 50;
    int numPoints = 40;
    int windowSize = 40;
    double weightElasticity = 0.02;
    double weightSmoothness = 0.05;
    int iterations = 500;
    cv::Mat drawing;

    Contour contour = getInitialContour(offset, imageSize, numPoints);
    Snake snake(contour);
    snake.updateImage(image);


    if (!cap.isOpened()) {
        cout << "cannot open camera";
    } else {
        for (int i = 0; i < iterations; i++) {
            cap >> image;
            snake.updateImage(image);

            double contoursDifference = snake.step(windowSize, weightElasticity, weightSmoothness, numPoints);
            cout << contoursDifference << endl;

            drawing = image.clone(); // Clean image
            displayContour(drawing, snake.getContour());

            auto energyImage = snake.getEnergyImage();
            cv::imshow("contour", drawing);
            cv::imshow("energy", energyImage);

            auto key = cv::waitKey(1);
            int ESCAPE_KEY = 27;
            if ((key & 0xEFFFFF) == ESCAPE_KEY) {
                break;
            }
        }
    }
    cv::destroyAllWindows();
}


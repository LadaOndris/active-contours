#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <contour.h>
#include <snake.h>
#include <examples/base.h>
#include <Runner.h>

int main() {
    Runner runner;
    RunParams params;
    params.colorOfInterest = cv::Point3i(246, 4, 70);
    params.removeBackground = true;
    params.morphDilate = true;
    params.morphErode = true;

    runner.startVideo("data/red_car.mkv", params);
    // startImageSnake("data/coin.jpeg");
    return 0;
}


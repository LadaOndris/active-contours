
#ifndef SNAKES_RUNNER_H
#define SNAKES_RUNNER_H

#include <iostream>
#include <opencv2/core.hpp>
#include "RunParams.h"

class Runner {
public:
    Runner();
    ~Runner();
    void startVideo(const std::string &videoPath, const RunParams &params);
    void startImage(const std::string &imagePath, const RunParams &params);
private:
    static void remove_background(const cv::Mat &src, cv::Mat dst, cv::Point3i color);
};


#endif //SNAKES_RUNNER_H

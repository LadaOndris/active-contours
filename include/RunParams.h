
#ifndef SNAKES_RUNPARAMS_H
#define SNAKES_RUNPARAMS_H

#include <opencv2/core.hpp>

class RunParams {
public:
    std::string videoPath;
    std::string imagePath;
    std::string saveMaskPath;
    int offsetROI = 150;
    int numPoints = 30;
    int windowSize = 28;
    double weightElasticity = 0.000001;
    double weightSmoothness = 0.01;

    int morphSizeDilate = 24;
    int morphSizeErode = 12;
    cv::Point3i colorOfInterestRGB;

    // Enable or disable functionality
    bool removeBackground = false;
    bool morphDilate = false;
    bool morphErode = false;

    int sleep = 1; // Sleeping in milliseconds between frames
};


#endif //SNAKES_RUNPARAMS_H


#ifndef SNAKES_RUNPARAMS_H
#define SNAKES_RUNPARAMS_H

#include <opencv2/core.hpp>

class RunParams {
public:
    int offsetROI = 150;
    int numPoints = 30;
    int windowSize = 28;
    double weightElasticity = 0.000001;
    double weightSmoothness = 0.01;

    int morphSizeDilate = 24;
    int morphSizeErode = 12;
    cv::Point3i colorOfInterest;

    // Enable or disable functionality
    bool removeBackground = false;
    bool morphDilate = false;
    bool morphErode = false;
};


#endif //SNAKES_RUNPARAMS_H

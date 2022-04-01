
#include <Runner.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <contour.h>
#include <snake.h>
#include <examples/base.h>

Runner::Runner() {
    cv::namedWindow("contour", cv::WINDOW_NORMAL);
    cv::namedWindow("energy", cv::WINDOW_NORMAL);


}

Runner::~Runner() {
    cv::destroyAllWindows();
}


void Runner::startVideo(const std::string &videoPath, const RunParams &params) {
    cv::VideoCapture cap(videoPath);
    cv::Mat image, drawing, postprocesedImage;
    cap >> image;
    postprocesedImage = image.clone();

    cv::Size imageSize = image.size();
    Contour contour = getInitialContour(params.offsetROI, imageSize, params.numPoints);
    Snake snake(contour);
    snake.updateImage(image);

    cv::Mat elementDilate = getStructuringElement(
            cv::MORPH_RECT, cv::Size(2 * params.morphSizeDilate + 1, 2 * params.morphSizeDilate + 1),
            cv::Point(params.morphSizeDilate, params.morphSizeDilate));
    cv::Mat elementErode = getStructuringElement(
            cv::MORPH_RECT, cv::Size(2 * params.morphSizeErode + 1, 2 * params.morphSizeErode + 1),
            cv::Point(params.morphSizeErode, params.morphSizeErode));

    if (!cap.isOpened()) {
        cout << "cannot open camera";
    } else {
        while (true) {
            cap >> image;
            if (image.empty())
                break;

            if (params.removeBackground) {
                remove_background(image, postprocesedImage, params.colorOfInterest);
            }
            else {
                postprocesedImage = image;
            }

            if (params.morphDilate) {
                cv::morphologyEx(postprocesedImage, postprocesedImage, cv::MORPH_DILATE, elementDilate);
            }
            if (params.morphErode) {
                cv::morphologyEx(postprocesedImage, postprocesedImage, cv::MORPH_ERODE, elementErode);
            }

            snake.updateImage(postprocesedImage);

            double contoursDifference = snake.step(params.windowSize,
                                                   params.weightElasticity,
                                                   params.weightSmoothness,
                                                   params.numPoints);
            cout << contoursDifference << endl;

            drawing = image.clone(); // Clean image
            displayContour(drawing, snake.getContour());

            auto energyImage = snake.getEnergyImage();
            cv::imshow("contour", drawing);
            cv::imshow("energy", energyImage);

            //            if (i == 50) {
            //                cv::imwrite("saved_examples/redcar_with_background_contour.png", drawing);
            //                cv::imwrite("saved_examples/redcar_with_background_energy.png", energyImage);
            //            }

            auto key = cv::waitKey(25);
            int ESCAPE_KEY = 27;
            if ((key & 0xEFFFFF) == ESCAPE_KEY) {
                break;
            }
        }
        cap.release();
    }
}

void Runner::startImage(const std::string &imagePath, const RunParams &params, int iterations) {

}

void Runner::remove_background(const cv::Mat &src, cv::Mat dst, cv::Point3i color) {
    int interval = 30;

    for (int y = 0; y < src.rows; y++) {
        for (int x_bgr = 0; x_bgr < src.cols * 3 + 2; x_bgr += 3) {
            auto b = src.at<uchar>(y, x_bgr);
            auto g = src.at<uchar>(y, x_bgr + 1);
            auto r = src.at<uchar>(y, x_bgr + 2);

            if (r > color.x - interval && r < color.x + interval &&
            g > color.y - interval && g < color.y + interval &&
            b > color.z - interval && b < color.z + interval) {

                dst.at<uchar>(y, x_bgr) = b;
                dst.at<uchar>(y, x_bgr + 1) = g;
                dst.at<uchar>(y, x_bgr + 2) = r;
            } else {
                dst.at<uchar>(y, x_bgr) = 0;
                dst.at<uchar>(y, x_bgr + 1) = 0;
                dst.at<uchar>(y, x_bgr + 2) = 0;
            }
        }
    }
}




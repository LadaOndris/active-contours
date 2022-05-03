
#include <Runner.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <contour.h>
#include <snake.h>
#include <examples/base.h>

Runner::Runner() {

}

Runner::~Runner() {
}


std::vector<std::vector<cv::Point>> Runner::startVideo(const std::string &videoPath, const RunParams &params) {
    std::vector<std::vector<cv::Point>> contours;

    cv::namedWindow("contour", cv::WINDOW_NORMAL);

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
                remove_background(image, postprocesedImage, params.colorOfInterestRGB);
            } else {
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
            if (contoursDifference == -1) {
                break;
            }
            drawing = image.clone(); // Clean image
            displayContour(drawing, snake.getContour());
            auto points = snake.getContour().getPoints();
            contours.push_back(points);
            // cv::drawContours(drawing, contours, -1, cv::Scalar(255, 0, 0), -1);

            auto energyImage = snake.getEnergyImage();
            cv::imshow("contour", drawing);

            //            if (i == 50) {
            //                cv::imwrite("saved_examples/redcar_with_background_contour.png", drawing);
            //                cv::imwrite("saved_examples/redcar_with_background_energy.png", energyImage);
            //            }

            auto key = cv::waitKey(params.sleep);
            int ESCAPE_KEY = 27;
            if ((key & 0xEFFFFF) == ESCAPE_KEY) {
                break;
            }
        }
        cap.release();
    }
    cv::destroyAllWindows();
    return contours;
}

void Runner::startImage(const std::string &imagePath, const RunParams &params) {
    cv::namedWindow("contour", cv::WINDOW_NORMAL);
    cv::Mat image = cv::imread(imagePath);
    cv::Mat postprocessedImage = image.clone();
    cv::Mat drawing;
    cv::Size imageSize = image.size();

    cv::Mat elementDilate = getStructuringElement(
            cv::MORPH_RECT, cv::Size(2 * params.morphSizeDilate + 1, 2 * params.morphSizeDilate + 1),
            cv::Point(params.morphSizeDilate, params.morphSizeDilate));
    cv::Mat elementErode = getStructuringElement(
            cv::MORPH_RECT, cv::Size(2 * params.morphSizeErode + 1, 2 * params.morphSizeErode + 1),
            cv::Point(params.morphSizeErode, params.morphSizeErode));


    if (params.removeBackground) {
        remove_background(image, postprocessedImage, params.colorOfInterestRGB);
    }
    if (params.morphDilate) {
        cv::morphologyEx(postprocessedImage, postprocessedImage, cv::MORPH_DILATE, elementDilate);
    }
    if (params.morphErode) {
        cv::morphologyEx(postprocessedImage, postprocessedImage, cv::MORPH_ERODE, elementErode);
    }


    Contour contour = getInitialContour(params.offsetROI, imageSize, params.numPoints);
    Snake snake(contour);
    snake.updateImage(postprocessedImage);

    while (true) {
        double contoursDifference = snake.step(params.windowSize,
                                               params.weightElasticity,
                                               params.weightSmoothness,
                                               params.numPoints);
        if (contoursDifference == -1) {
            break;
        }
        drawing = image.clone(); // Clean image
        displayContour(drawing, snake.getContour());

        cv::imshow("contour", drawing);

        auto key = cv::waitKey(params.sleep);
        int ESCAPE_KEY = 27;
        if ((key & 0xEFFFFF) == ESCAPE_KEY) {
            break;
        }
    }

    int ESCAPE_KEY = 27;
    while ((cv::waitKey() & 0xEFFFFF) != ESCAPE_KEY);

    cv::destroyAllWindows();
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

void Runner::saveContoursAsMasks(std::vector<std::vector<cv::Point>> contours, std::string savePath,
                                 std::string inputVideoPath) {
    cv::VideoCapture in_video(inputVideoPath);
    int width = (int)in_video.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = (int)in_video.get(cv::CAP_PROP_FRAME_HEIGHT);
    int in_fps = (int)in_video.get(cv::CAP_PROP_FPS);
    int fourcc = cv::VideoWriter::fourcc('F', 'L', 'V', '1');
    cv::VideoWriter out_video(savePath, fourcc, in_fps, cv::Size(width, height), false);

    cv::Mat mask(height, width, CV_8UC1);
    std::vector<std::vector<cv::Point>> frame_contours;

    int contour_id = 0;
    for (auto &contour : contours) {
        std::cout << "Saving mask of the frame " << contour_id + 1 << "/" << contours.size() << std::endl;
        contour_id++;
        frame_contours.clear();
        frame_contours.push_back(contour);
        mask = cv::Mat::zeros(height, width, CV_8UC1);
        cv::drawContours(mask, frame_contours, -1, cv::Scalar(255, 0, 0), -1);
        out_video.write(mask);
        // Clear output line
        if (contour_id < contours.size()) {
            std::cout << "\x1b[A";
        }
    }
    in_video.release();
    out_video.release();
    std::cout << "Saved all frames." << std::endl;
}


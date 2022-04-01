#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <contour.h>
#include <snake.h>
#include <librealsense2/rs.hpp>
#include <librealsense2/hpp/rs_internal.hpp>
#include <examples/base.h>

void startDepthcamSnake() {
    // Setup RealSense image source
    rs2::context context;
    if (context.query_devices().size() == 0) {
        throw std::runtime_error("No Intel RealSense device connected");
    }
    std::cout << "hello from librealsense - " << RS2_API_VERSION_STR << std::endl;
    std::cout << "You have " << context.query_devices().size() << " RealSense devices connected" << std::endl;

    rs2::device device = context.query_devices()[0];
    std::cout << "Using device 0: " << device.get_info(rs2_camera_info::RS2_CAMERA_INFO_NAME) << std::endl;

    rs2::config config;
    config.enable_stream(rs2_stream::RS2_STREAM_DEPTH, 640, 480, rs2_format::RS2_FORMAT_Z16);

    rs2::pipeline pipeline(context);
    pipeline.start(config);
    // cv::Mat image = cv::Mat(480, 640, CV_16UC1);
    cv::Mat image_colormap;


    //
    cv::namedWindow("main", cv::WINDOW_NORMAL);
    cv::Size imageSize(640, 480);
    int offset = 50;
    int numPoints = 50;
    int windowSize = 20;
    double weightElasticity = 0.1;
    double weightSmoothness = 0.1;
    int iterations = 1000;
    cv::Mat drawing;

    Contour contour = getInitialContour(offset, imageSize, numPoints);
    Snake snake(contour);
    //snake.updateImage(image);


    for (int i = 0; i < iterations; i++) {
        rs2::frameset frameset = pipeline.wait_for_frames();
        rs2::depth_frame frame = frameset.get_depth_frame();
        cv::Mat image(imageSize, CV_16UC1, (void *) frame.get_data(), cv::Mat::AUTO_STEP);

        //cv::cvtColor(image, image, cv::COLOR_GRAY2RGB);

        cv::convertScaleAbs(image, image_colormap, 0.2);
        cv::equalizeHist(image_colormap, image_colormap);
        cv::applyColorMap(image_colormap, image_colormap, cv::COLORMAP_JET);

        snake.updateImage(image);
        snake.step(windowSize, weightElasticity, weightSmoothness, numPoints);

        drawing = image_colormap.clone(); // Clean image
        displayContour(drawing, snake.getContour());

        auto energyImage = snake.getEnergyImage();
        cv::imshow("main", energyImage);
        auto key = cv::waitKey(25);
        int ESCAPE_KEY = 27;
        if ((key & 0xEFFFFF) == ESCAPE_KEY) {
            break;
        }
    }

    cv::destroyAllWindows();
}

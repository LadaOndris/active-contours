#include <iostream>
#include <contour.h>
#include <Runner.h>

void tokenize(std::string const &str, const char delim,
              std::vector<std::string> &out) {
    std::stringstream ss(str);
    std::string s;
    while (std::getline(ss, s, delim)) {
        out.push_back(s);
    }
}

int parseArgs(int argc, char **argv, RunParams &params) {
    vector<string> args(argv + 1, argv + argc);

    for (std::vector<string>::size_type i = 0; i < args.size(); i += 2) {
        if (i + 1 == args.size()) {
            cout << "Expected parameter value" << endl;
            return 3;
        }
        auto nextArg = args[i + 1].c_str();

        if (args[i] == "--video") {
            params.videoPath = args[i + 1];
        } else if (args[i] == "--image") {
            params.imagePath = args[i + 1];
        } else if (args[i] == "--offsetROI") {
            params.offsetROI = atoi(nextArg);
        } else if (args[i] == "--numPoints") {
            params.numPoints = atoi(nextArg);
        } else if (args[i] == "--windowSize") {
            params.windowSize = atoi(nextArg);
        } else if (args[i] == "--morphSizeDilate") {
            params.morphSizeDilate = atoi(nextArg);
        } else if (args[i] == "--morphSizeErode") {
            params.morphSizeErode = atoi(nextArg);
        } else if (args[i] == "--weightElasticity") {
            params.weightElasticity = atof(nextArg);
        } else if (args[i] == "--weightSmoothness") {
            params.weightSmoothness = atof(nextArg);
        } else if (args[i] == "--colorOfInterestRGB") {
            std::vector<std::string> parts;
            tokenize(args[i + 1], ',', parts);
            if (parts.size() != 3) {
                cout << "Invalid color. Expected pattern is R,G,B" << endl;
                return 1;
            }
            int r = atoi(parts[0].c_str());
            int g = atoi(parts[1].c_str());
            int b = atoi(parts[2].c_str());
            params.colorOfInterestRGB = cv::Point3i(r, g, b);
            params.removeBackground = true;
        } else if (args[i] == "--morphDilate") {
            params.morphDilate = args[i + 1] == "true";
        } else if (args[i] == "--morphErode") {
            params.morphErode = args[i + 1] == "true";
        } else if (args[i] == "--sleep") {
            params.sleep = atoi(nextArg);
        }
        else {
            cout << "Unknown option: " << args[i] << endl;
            return 2;
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    Runner runner;
    RunParams params;
    int code = parseArgs(argc, argv, params);
    if (code != 0) {
        return code;
    }

    if (!params.videoPath.empty()) {
        runner.startVideo(params.videoPath, params);
    } else if (!params.imagePath.empty()) {
        runner.startImage(params.imagePath, params);
    }
    return 0;
}


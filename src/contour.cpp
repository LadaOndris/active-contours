//
// Created by lada on 3/10/22.
//

#include "contour.h"
#include <stdexcept>

Contour::Contour(vector<cv::Point> contour_points) : points(contour_points) {

}

vector<cv::Point> Contour::getPoints() const {
    return points;
}

void Contour::removeDuplicatePoints() {
    double distThreshold = 3;
    vector<cv::Point> nonDuplicates;

    int i = 0;
    int j = 1;
    while (j != points.size()) {
        if (pointsDist(points[i], points[j]) < distThreshold) {
            // The two points are at the same position.
            // Ignore the point, and move further.
            j++;
        }
        else {
            // Points are in different position.
            // Add the second point.
            nonDuplicates.push_back(points[j]);
            i = j;
            j++;
        }
    }

    // j % points.size() == 0
    if (pointsDist(points[i], points[0]) > distThreshold) {
        nonDuplicates.push_back(points[0]);
    }

    points = nonDuplicates;
}

void Contour::samplePointsUniformly(int num_points) {
    removeDuplicatePoints();

    // Determine contour's length
    double contourLength = getContourLength();
    double uniformDist = contourLength / num_points;
    vector<cv::Point> uniformPoints;

    int lineIndex = 0;
    int uniformPointIndex = 0;
    cv::Point startLinePoint = getPointByIndexWithOverflow(0);
    cv::Point endLinePoint = getPointByIndexWithOverflow(1);
    cv::Point currentLinePoint = startLinePoint;
    double distanceToUniformPoint = uniformDist;

    while (uniformPointIndex < num_points) {

        double remainingUniformDistance = moveToNewUniformPosition(startLinePoint, endLinePoint,
                                                                   currentLinePoint, distanceToUniformPoint);

        // The line is not long enough, still part of the uniform distance remains
        if (remainingUniformDistance > 0.01) {
            // Move to the next line
            lineIndex++;
            startLinePoint = getPointByIndexWithOverflow(lineIndex);
            endLinePoint = getPointByIndexWithOverflow(lineIndex + 1);
            currentLinePoint = startLinePoint;
            distanceToUniformPoint = remainingUniformDistance;
        } else {
            // The point is placed, move to another one
            uniformPointIndex++;
            uniformPoints.push_back(currentLinePoint);
            distanceToUniformPoint = uniformDist;
        }
    }
    points = uniformPoints;
}

double Contour::moveToNewUniformPosition(const cv::Point startLinePoint, const cv::Point endLinePoint,
                                         cv::Point &currentLinePoint, double moveDist) {
    int dx_raw = endLinePoint.x - startLinePoint.x;
    int dy_raw = endLinePoint.y - startLinePoint.y;
    cv::Point directionVector(dx_raw, dy_raw);
    cv::Point vectorShift = moveDist / l2norm(directionVector) * directionVector;
    cv::Point uniformPointCandidate = currentLinePoint + vectorShift;

    // If the uniform point is not farther than the end point
    if (pointsDist(startLinePoint, uniformPointCandidate) <= pointsDist(startLinePoint, endLinePoint)) {
        currentLinePoint.x = uniformPointCandidate.x;
        currentLinePoint.y = uniformPointCandidate.y;
        return 0;
    }
    else {
        double remainingDist = pointsDist(uniformPointCandidate, endLinePoint);
        currentLinePoint.x = endLinePoint.x;
        currentLinePoint.y = endLinePoint.y;
        return remainingDist;
    }
}

cv::Point Contour::getPointByIndexWithOverflow(int index) const {
    auto modVal = points.size();
    auto newIndex = ((index % modVal) + modVal) % modVal;
    return points[newIndex];
}

double Contour::getContourLength() const {
    double length = 0;
    for (int i = 0; i < points.size() - 1; i++) {
        if (i == 0) {
            length += pointsDist(points[points.size() - 1], points[0]);
        }
        length += pointsDist(points[i], points[i + 1]);
    }
    return length;
}

double Contour::l2norm(cv::Point p) const {
    return sqrt(p.x * p.x + p.y * p.y);
}

double Contour::pointsDist(cv::Point a, cv::Point b) const {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

double Contour::difference(const Contour &other) const {
    auto otherPoints = other.getPoints();
    if (points.size() != otherPoints.size()) {
        throw std::invalid_argument("The number of points in the two contours do not match.");
    }
    double difference = 0;
    for (int i = 0; i < points.size(); i++) {
        difference += pointsDist(points[i], otherPoints[i]);
    }
    return difference;
}

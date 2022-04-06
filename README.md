# Active contours


## Description

Active contours is an iterative algorithm used for finding object boundaries. 

The program is written in C++ as a command-line utility that uses OpenCV for image
processing tasks and displaying results.

It supports both images and videos.


## Prerequisites

* C++
* OpenCV
* Cmake, Make

## Examples

### Detecting a coin in an image
```
./cmake-build-debug/src/snakes_run --image data/coin.jpeg --windowSize 10 --offsetROI 60 --numPoints 40 --weightSmoothness 0.1 --weightElasticity 0.1
```
<p float="left">
    <img src="https://user-images.githubusercontent.com/22337362/162012209-7154f1b2-c0a1-4314-af46-cd623d6c1872.png" alt="coin" width="520"/>
</p> 

### Tracking a red car in a video
```
./cmake-build-debug/src/snakes_run --video data/red_car.mkv --colorOfInterestRGB 240,10,70 --morphDilate true --morphErode true --numPoints 30
```
<p float="left">
    <img src="https://user-images.githubusercontent.com/22337362/162012174-f9d1cfd4-7eef-4a01-851b-218e43b9ea43.png" alt="red_car" width="520"/>
</p> 

### Experimenting with different weights
High smoothness, low elasticity:

```
./cmake-build-debug/src/snakes_run --image data/four_circles.png --windowSize 10 --offsetROI 60 --numPoints 40 --weightSmoothness 0.1 --weightElasticity 0.0001
```
<p float="left">
    <img src="https://user-images.githubusercontent.com/22337362/162012101-28924e1d-f9ea-464c-886b-cf7bc17cc3d5.png" alt="four_circles" width="520"/>
</p> 
Low smoothness, high elasticity:

```
./cmake-build-debug/src/snakes_run --image data/four_circles.png --windowSize 10 --offsetROI 60 --numPoints 40 --weightSmoothness 0.0001 --weightElasticity 0.1
```
<p float="left">
    <img src="https://user-images.githubusercontent.com/22337362/162016941-ee0b04c4-fac6-493a-9d8e-766ea9b8bbbc.png" alt="four_circles" width="520"/>
</p> 


## Program arguments


The following arguments are supported:

* **--video string** Path to the target video 
* **--image string** Path to the target image
* **--offsetROI int** The initialization position of the contour as the distance from the images' border (default 150)
* **--numPoints int** The number of control points of a contour (default 30)
* **--windowSize int** The size of window $W$ around control point $v_i$ in pixels (default 28)
* **--weightElasticity double** The weight controlling the importance of elasticity in the total sum of energies (default 0.000001)
* **--weightSmoothness double** The weight controlling the importance of smoothness in the total sum of energies (default 0.01)
* **--colorOfInterestRGB int,int,int** If set, all colors except this one are replaced with black (no default)
* **--morphDilate bool** Whether to dilate the image (default false)
* **--morphSizeDilate int** The size of morphing element (default 24)
* **--morphErode bool** Whether to erode the image (default false)
* **--morphSizeErode int** The size of morphing element (default 12)
* **--sleep int** The wait interval between frames (default 1)

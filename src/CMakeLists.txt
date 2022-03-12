cmake_minimum_required(VERSION 3.22)
project(snakes)

set(CMAKE_CXX_STANDARD 17)
set(BINARY ${CMAKE_PROJECT_NAME})

find_package(realsense2 REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(/usr/include/librealsense2/)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)
set(SOURCES ${SOURCES})
add_executable(${BINARY}_run ${SOURCES})
add_library(${BINARY}_lib STATIC ${SOURCES})

target_link_libraries(${BINARY}_run ${OpenCV_LIBS} ${realsense2_LIBRARY})
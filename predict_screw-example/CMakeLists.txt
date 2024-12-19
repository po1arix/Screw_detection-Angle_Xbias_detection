cmake_minimum_required(VERSION 3.27)
project(SDori)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(SDori
        ScrewDetection.h
        ScrewDetection.cpp
        Test.cpp
)
target_link_libraries(SDori ${OpenCV_LIBS})
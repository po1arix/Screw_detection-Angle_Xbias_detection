cmake_minimum_required(VERSION 3.27)
project(1204orb_demo)

set(CMAKE_CXX_STANDARD 17)
#set(OpenCV_DIR "C:\\Program Files\\opencv\\opencv\\build")  # opencv/build目录
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(1204orb_demo
        AngleWithXbias.cpp
        AngleWithXbias.hpp
        nlohmann/json.hpp
#        large_scale_test.cpp
)

target_link_libraries(1204orb_demo ${OpenCV_LIBS})
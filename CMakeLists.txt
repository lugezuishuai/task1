cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
project(xianshituxiang)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(shenfu shenfu.cpp)
target_link_libraries(shenfu ${OpenCV_LIBS})

add_executable(zhuangjiapian1 zhuangjiapian1.cpp)
target_link_libraries(zhuangjiapian1 ${OpenCV_LIBS})


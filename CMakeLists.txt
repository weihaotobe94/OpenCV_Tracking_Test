cmake_minimum_required(VERSION 2.6)
project(opencv_tracker)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(opencv_tracker main.cpp)

target_link_libraries(opencv_tracker ${OpenCV_LIBS})

install(TARGETS opencv_tracker RUNTIME DESTINATION bin)

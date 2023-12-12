cmake_minimum_required(VERSION 3.27)
project(lab5)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-O3)

add_executable(lab5 main.cpp)

target_link_libraries(lab5 PRIVATE
        libopencv_core.so
        libopencv_highgui.so
        libopencv_video.so
        libopencv_videoio.so
        libopencv_imgproc.so
        libopencv_calib3d.so
        libopencv_features2d.so
        libopencv_flann.so
        libopencv_gapi.so
        libopencv_imgcodecs.so
)
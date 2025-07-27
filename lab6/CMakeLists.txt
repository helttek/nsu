cmake_minimum_required(VERSION 3.27)
project(lab6)

set(CMAKE_CXX_STANDARD 17)

#add_compile_options(`pkg-config --libs --cflags libusb-1.0`)

add_executable(lab6 main.cpp)

#target_link_libraries(
#        libusb-1.0.so
#)
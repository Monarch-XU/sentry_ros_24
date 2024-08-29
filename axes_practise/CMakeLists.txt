cmake_minimum_required(VERSION 3.26)
project(axes_practise)

set(CMAKE_CXX_STANDARD 17)
set(Opencv_DIR "D:\\opencv\\mingw_build")
find_package(OpenCV REQUIRED)

add_executable(axes_practise main.cpp)
target_link_libraries(axes_practise ${OpenCV_LIBS})
cmake_minimum_required(VERSION 3.1)

project(COP290_A1)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
add_executable(main main.cpp)
target_link_libraries(main PRIVATE ${OpenCV_LIBS})
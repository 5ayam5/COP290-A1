cmake_minimum_required(VERSION 3.1)

project(COP290_A1)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
add_executable(computeDensities computeDensities.cpp)
target_link_libraries(computeDensities PRIVATE ${OpenCV_LIBS})
add_executable(computeDensities_subsampling computeDensities_subsampling.cpp)
target_link_libraries(computeDensities_subsampling PRIVATE ${OpenCV_LIBS})
add_executable(correctPerspective correctPerspective.cpp)
target_link_libraries(correctPerspective PRIVATE ${OpenCV_LIBS})

cmake_minimum_required(VERSION 2.8)

project(slic)
project (eigen)

find_package( OpenCV 3.3.0 REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( eigen __eigen.cpp __eigenHelpers.cpp _eigenframes.cpp )
add_executable( slic __slic.cpp _slicAlgorithm.cpp)

target_link_libraries( eigen ${OpenCV_LIBS})
target_link_libraries( slic  ${OpenCV_LIBS})





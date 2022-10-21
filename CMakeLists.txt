# 
# 这个是外面的cmake
# 

cmake_minimum_required(VERSION 3.16.0)

project(minikv VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(src)

enable_testing()

add_subdirectory(test)

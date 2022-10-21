# 
# 这个目录是库
# 

cmake_minimum_required(VERSION 3.16.0)

project(minikv VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(kvclient)
add_subdirectory(kvserver)

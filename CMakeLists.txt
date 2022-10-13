# 
# 这个是外面的cmake
# 

cmake_minimum_required(VERSION 3.16.0)

project(minikv VERSION 1.0)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)

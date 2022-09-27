cmake_minimum_required(VERSION 3.16.3)


project(miniDB)

set(CMAKE_CXX_COMPILER "c++")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS -g -Wall -Werror)


# 头文件目录
include_directories(/include)

aux_source_directory(. ALL_SOURCES)

add_executable(miniDB ${ALL_SOURCES})
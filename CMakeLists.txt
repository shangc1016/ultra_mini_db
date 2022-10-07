cmake_minimum_required(VERSION 3.16.0)

project(miniDB VERSION 1.0)

file(GLOB SOURCES src/*.cpp)

add_compile_options(-std=c++14 -Werror -Wall -Wextra)

add_executable(miniDB ${SOURCES})

target_include_directories(miniDB PRIVATE ${PROJECT_SOURCE_DIR}/include)


message("-- CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER_ID}")
message("-- CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
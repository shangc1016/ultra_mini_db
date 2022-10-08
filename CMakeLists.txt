cmake_minimum_required(VERSION 3.16.0)

project(miniDB VERSION 1.0)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

file(GLOB SOURCES src/*.cc)

add_compile_options(
    -std=c++14 
    -Werror 
    -Wall 
    -Wextra
)


link_directories(
    "${PROJECT_SOURCE_DIR}/3rdparty/lib"   # 第三方动态库的位置
)

link_libraries(cityhash)                   # 第三方动态库的名称

add_executable(miniDB ${SOURCES})

target_include_directories(miniDB PRIVATE 
    ${PROJECT_SOURCE_DIR}/include          # 本项目的头文件
    ${PROJECT_SOURCE_DIR}/3rdparty/include # 三方库的头文件
)


message(${SOURCES})
message("-- CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER_ID}")
message("-- CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")


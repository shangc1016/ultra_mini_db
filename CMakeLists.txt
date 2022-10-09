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

# 第三方动态库的位置
link_directories("/usr/local/lib")


add_executable(miniDB ${SOURCES})


# 项目依赖的库文件名字
target_link_libraries(miniDB cityhash pthread)


# cmake打印信息
message("-- CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER_ID}")
message("-- CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")


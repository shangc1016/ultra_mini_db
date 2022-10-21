# 
# 这个目录是kvserver
# 


cmake_minimum_required(VERSION 3.16.0)

project(kvserver VERSION 1.0)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)


file(GLOB SOURCES *.cc ../*.cc ../../3rdparty/*.cc)

add_compile_options(
    -std=c++14
    -g
    -O0
    -Werror
    -Wall
    -fno-omit-frame-pointer    # 打印函数调用路径
    -fsanitize=address
    # -fsanitize=memory
    # -fsanitize=leak
    # -fsanitize=thread
    # -fsanitize=undefined
)

# 第三方动态库的位置
link_directories("/usr/local/lib")

add_executable(kvserver ${SOURCES})

link_libraries(-fsanitize=address)


# 项目依赖的库文件名字
target_link_libraries(kvserver asan cityhash pthread)


# cmake打印信息
message("-- CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER_ID}")
message("-- CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")


# 
#  这个目录是gtest测试
# 


cmake_minimum_required(VERSION 3.16.0)

project(test_minikv VERSION 1.0)


set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)


file(GLOB SOURCES *.cc ../src/*.cc ../3rdparty/*.cc)

add_compile_options(
    -std=c++14 
    -Werror 
    -Wall 
    -Wextra 
    -g
    -O0
    -fno-omit-frame-pointer    # 打印函数调用路径
)

# 第三方动态库的位置
link_directories("/usr/local/lib")


add_executable(test_minikv ${SOURCES})


# 项目依赖的库文件名字
# gtest需要用到pthread
target_link_libraries(test_minikv gtest pthread cityhash)


message(${SOURCES})
# cmake打印信息
message("-- CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER_ID}")
message("-- CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")


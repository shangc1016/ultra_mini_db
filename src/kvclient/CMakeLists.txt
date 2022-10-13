# 
# 这个目录是kvclient
# 


cmake_minimum_required(VERSION 3.16.0)

project(kvclient VERSION 1.0)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)


file(GLOB SOURCES *.cc ../*.cc ../../3rdparty/*.cc)

add_compile_options(
    -std=c++14
    -Werror 
    -Wall 
    -Wextra
    -fno-omit-frame-pointer    # 打印函数调用路径
    -fsanitize=address
    # -fsanitize=memory
    # -fsanitize=leak
    # -fsanitize=thread
    # -fsanitize=undefined
)

# 第三方动态库的位置
link_directories("/usr/local/lib")

add_executable(kvclient ${SOURCES})

link_libraries(-fsanitize=address)


# 项目依赖的库文件名字
target_link_libraries(kvclient asan cityhash pthread)


# cmake打印信息
message("-- CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER_ID}")
message("-- CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")


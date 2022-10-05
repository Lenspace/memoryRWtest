cmake_minimum_required (VERSION 3.10)
SET(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_CXX_STANDARD 17)

set(Demo_VERSION_MAJOR 0)
set(Demo_VERSION_MINOR 3)

set(CMAKE_BUILD_TYPE Release)


# 项目信息
project(memRW LANGUAGES CXX)

# 增加编译选项
set(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")
message(STATUS "option: CMAKE_CXX_FLAGS")  # 提示信息
message(STATUS "内存随机读写测试")      # 测试内存随机读写效率

# 查找当前目录下所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

add_executable(memRW ${DIR_SRCS})

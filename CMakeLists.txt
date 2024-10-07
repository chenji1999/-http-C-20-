# 指定CMake的最低版本要求
cmake_minimum_required(VERSION 3.14)

# 定义项目名称
project(httpserver_project)

# 设置C++标准版本
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 定义源文件
set(SOURCES http.cpp)

# 生成可执行文件
add_executable(httpserver ${SOURCES})
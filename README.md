# -基于C++20的自研协程库的http服务器-

协程暂时不支持，目前是version 0.1

环境Ubuntu 20

cmake 版本3.14以上

项目介绍：http.cpp http.h是两个文件，基于epoll的小服务器，测试函数是在测试http文件夹里，运行run.sh即可运行

服务器编译代码：

cmake -B build

cmake --build build

build/./httpserver
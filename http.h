#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <iostream>
#include <cstring>

namespace http_server{
     const int MAX_EVENTS = 1024;
     const int BUFFER_SIZE = 4096;
     const int SERVER_PORT = 8080;
     
    // 设置非阻塞模式
    int set_non_blocking(int fd);

    // 创建服务器套接字并绑定
    int create_server_socket(int port);

    // 初始化 epoll
    int init_epoll(int listen_fd);

    // 事件循环处理
    void event_loop(int epoll_fd, int listen_fd);
}

#include"http.h"
#include <sys/epoll.h>

namespace http_server{
    //read fd and set to O_NONBLOCK
    int set_non_blocking(int fd){
        int flags = fcntl(fd,F_GETFL,0);
        fcntl(fd,F_SETFL,flags | O_NONBLOCK);
        return 0;
    }
    //create fd for server to listen with port
    int create_server_socket(int port){
        int listen_fd = socket(AF_INET ,SOCK_STREAM,0);
        if(listen_fd==-1){
            std::cerr<<"Fail to create socket"<<"\n";
            exit(EXIT_FAILURE);
        }
        int opt =1;
        setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
        //socketaddr init
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        if(bind(listen_fd,(sockaddr *)&server_addr,sizeof(server_addr))==-1){
            std::cerr<<"fail to bind\n";
            exit(EXIT_FAILURE);
        }
        if(listen(listen_fd,SOMAXCONN)==-1){
            std::cerr<<"Listen failed\n";
            exit(EXIT_FAILURE);
        }
        set_non_blocking(listen_fd);
        return listen_fd;
    }
    /*
    author:chenji
    use:init epoll fd with listen_fd
    */
    int init_epoll(int listen_fd){
        int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
        if(epoll_fd == -1){
            std::cerr<<"Failed create epoll file\n";
            exit(EXIT_FAILURE);
        }
        epoll_event event{};
        event.data.fd = listen_fd;
        event.events = EPOLLET | EPOLLIN;
        if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listen_fd,&event)==-1){
            std::cerr<<"Failed to add listen_fd to epoll\n";
            exit(EXIT_FAILURE);
        }
        return epoll_fd;
    }
    // event loop to build fd for connection
    void event_loop(int epoll_fd, int listen_fd){
        epoll_event events[MAX_EVENTS];
        while(true){
            int num_events = epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
            for(int i = 0; i<num_events;++i){
                int fd =  events[i].data.fd;
                if(fd == listen_fd){
                    while(true){
                        sockaddr_in client_addr{};
                        socklen_t client_len = sizeof(client_addr);
                        int client_fd = accept(listen_fd,(sockaddr*)&client_addr,&client_len);
                        if(client_fd == -1){
                            if(errno == EAGAIN ||errno==EWOULDBLOCK){
                                break;
                            }else{
                                std::cerr<<"Accept failed\n";
                                exit(EXIT_FAILURE);
                            }
                        }
                        set_non_blocking(client_fd);
                        epoll_event event{};
                        event.data.fd = client_fd;
                        event.events = EPOLLET | EPOLLIN;
                        epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_fd,&event);
                        
                    }
                }else if(events[i].events&EPOLLIN){
                    char buffer[BUFFER_SIZE];
                    memset(buffer, 0, sizeof(buffer));
                    int bytes_read = read(fd, buffer, sizeof(buffer));
                    if (bytes_read <= 0) {
                    if (bytes_read == 0 || errno != EAGAIN) {
                        close(fd);  // 关闭连接
                    }
                    } else {
                    // 处理 HTTP 请求并返回响应
                    std::cout << "Received request:\n" << buffer << std::endl;

                    // 发送简单的 HTTP 响应
                    const char *response = 
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Length: 13\r\n"
                        "Content-Type: text/plain\r\n"
                        "\r\n"
                        "Hello, World!";
                    write(fd, response, strlen(response));
                    close(fd);  // 响应后关闭连接
                    }
                }
            }
        }
    }
}


int main() {
    int listen_fd = http_server::create_server_socket(http_server::SERVER_PORT);
    int epoll_fd = http_server::init_epoll(listen_fd);

    std::cout << "Server is listening on port " << http_server::SERVER_PORT << std::endl;
    http_server::event_loop(epoll_fd, listen_fd);

    close(listen_fd);
    close(epoll_fd);
    return 0;
}


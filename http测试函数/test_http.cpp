
#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class TestClient {
public:
    TestClient(const std::string& server_ip, int server_port) 
        : server_ip(server_ip), server_port(server_port) {}

    // 模拟客户端连接，发送请求，接收响应
    void connect_and_test() {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            std::cerr << "Socket creation failed!" << std::endl;
            return;
        }

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_port);
        inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

        if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            std::cerr << "Connection to server failed!" << std::endl;
            close(sock);
            return;
        }

        // 发送 HTTP 请求
        std::string request = 
            "GET / HTTP/1.1\r\n"
            "Host: " + server_ip + "\r\n"
            "Connection: close\r\n\r\n";

        if (send(sock, request.c_str(), request.size(), 0) == -1) {
            std::cerr << "Failed to send request!" << std::endl;
            close(sock);
            return;
        }

        // 接收服务器响应
        char buffer[4096];
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::cout << "Received response from server:\n" << buffer << std::endl;
        } else {
            std::cerr << "Failed to receive response!" << std::endl;
        }

        close(sock);
    }

private:
    std::string server_ip;
    int server_port;
};

class ConnectionTester {
public:
    ConnectionTester(const std::string& server_ip, int server_port, int num_threads)
        : server_ip(server_ip), server_port(server_port), num_threads(num_threads) {}

    // 启动多线程测试
    void start_test() {
        std::vector<std::thread> threads;

        // 创建并启动线程
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back([this]() {
                TestClient client(this->server_ip, this->server_port);
                client.connect_and_test();
            });
        }

        // 等待所有线程完成
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }

        std::cout << "All threads finished." << std::endl;
    }

private:
    std::string server_ip;
    int server_port;
    int num_threads;
};

int main() {
    std::string server_ip = "127.0.0.1";  // 本地服务器 IP
    int server_port = 8080;               // 服务器端口
    int num_threads = 10;                 // 模拟的客户端线程数量

    // 创建连接测试器
    ConnectionTester tester(server_ip, server_port, num_threads);
    
    // 启动测试
    tester.start_test();

    return 0;
}

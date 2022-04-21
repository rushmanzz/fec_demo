#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
//#include <sys/epoll.h>
#include <sys/time.h>
#include <chrono>
#include <assert.h>
#include <cstring>
#include <memory>
#include <signal.h>
#include "tcp_server.h"

uint32_t TCPServer::start_ts = 0;
uint32_t TCPServer::end_ts = 0;


void tcp_signal_handler(int signum){
    std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
    std::cout << "Time of receive the packet(tcp_server): " << TCPServer::end_ts - TCPServer::start_ts << "ms" <<std::endl;

    exit(signum);
}

TCPServer::TCPServer(std::string &ip, int port, int duration_ms):
_ip(ip),
_port(port),
len_t(1500),
_duration_ms(duration_ms),
recv_count(0) {

}

TCPServer::~TCPServer(){
    ::close(_fd);
}

int TCPServer::set_no_block(){
    int sock_flag = ::fcntl(_fd, F_GETFL);
    if (sock_flag < 0) {
        return sock_flag;
    }
    sock_flag |= O_NONBLOCK;
    ::fcntl(_fd, F_SETFL, sock_flag);
    return 0;
}

void TCPServer::bind(){
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    assert(_fd != -1);
    //int ret = set_no_block();
    //assert(ret != -1);
    //addr
    struct sockaddr_in addr;
    {
    ::bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    int ret = ::bind(_fd, (struct sockaddr*)&addr, sizeof(addr));
    assert(ret != -1);

}

void TCPServer::run(){
    ::signal(SIGINT,tcp_signal_handler);
    bind();
    int ret = ::listen(_fd,5);
    assert(ret != -1);
    int sock_fd = accept(_fd,nullptr,nullptr);
    assert(sock_fd != -1);
    std::cout << "accept a client!" << std::endl;

    unsigned char buf[len_t];

    int len = 0;

    while(true){
        ::bzero(&buf,len_t);

        len = recv(sock_fd, buf, len_t, _duration_ms);

        if(len == 0){
            continue;
        }

        if(len < 0){
            std::cout << "read error code" << len <<std::endl;
            continue;
        }
        if(TCPServer::start_ts == 0){
            start_ts = currentMs();
        }
        TCPServer::end_ts = currentMs();
        recv_count++;
        std::cout << "tcp recv count: " << recv_count << "    buf: " << buf << std::endl;
    }

}



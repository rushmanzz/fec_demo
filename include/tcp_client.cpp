#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <assert.h>
#include <cstring>
#include <memory>
#include <string>
#include <signal.h>
#include <assert.h>
#include "tcp_client.h"

TCPClient::TCPClient(std::string &ip, int port):
_ip(ip),
_port(port),
send_count(0) {

}

TCPClient::~TCPClient(){
    ::close(_fd);
}

int TCPClient::set_no_block(){
    int sock_flag = ::fcntl(_fd, F_GETFL);
    if (sock_flag < 0) {
        return sock_flag;
    }
    sock_flag |= O_NONBLOCK;
    ::fcntl(_fd, F_SETFL, sock_flag);
    return 0;
}

void TCPClient::connect(){
    _fd = socket(AF_INET,SOCK_STREAM,0);
    assert(_fd != -1);
    int ret;
    //int ret = set_no_block();
    //assert(ret != -1);

    ::bzero(&_addr,sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);
    assert(::inet_pton(AF_INET, _ip.c_str(), &_addr.sin_addr) != -1);

    ret = ::connect(_fd,((struct sockaddr*)&_addr),sizeof(_addr));
    assert(ret != -1);

}

void TCPClient::send(std::string &data){
    int ret = ::send(_fd,data.c_str(),data.length(),0);
    if(ret < 0){
        std::cout << "send msg error ! data: " << data << std::endl;
        return ;
    }
    send_count++;
    std::cout << "tcp send count :" << send_count << std::endl;
}
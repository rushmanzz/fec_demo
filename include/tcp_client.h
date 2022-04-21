#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <netinet/in.h>


class TCPClient {
public:
    TCPClient(std::string &ip, int port);

    ~TCPClient();

    int set_no_block();

    void connect();

    void send(std::string &data);

private:
    std::string _ip;
    int _port;
    int _fd; 
    struct sockaddr_in _addr;
    unsigned char _buf[1500];
    uint32_t send_count;

};


#endif
#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include<iostream>
#include<string>
#include <../util/util.h>

class TCPServer{
public:
    TCPServer(std::string &ip, int port, int duration_ts);

    ~TCPServer();

    void bind();

    void run();

    int set_no_block();

private:
    std::string _ip;
    int _port;
    int _fd;
    int _duration_ms;
    const int len_t;
    int recv_count;

public:
    static uint32_t start_ts;
    static uint32_t end_ts;

};


#endif
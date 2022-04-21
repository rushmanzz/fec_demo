

#ifndef UDP_SERVER_H
#define UDP_SERVER_H
#include <iostream>
#include <deque>
#include <vector>
#include <map>
#include <string>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <fec.h>
#include <../util/util.h>

class UDPServer {
public:
    UDPServer(std::string &ip, int port, int data_shards, int parity_shards, int duration_ms);

    ~UDPServer();

    int set_no_block();

    void bind();

    void run();

    void set_fec(int data_shards, int parity_shards);

    void msg_process();

    void decode_fec(std::pair<byte*, size_t>&);

private:
    std::string u_ip;
    int u_port;
    int u_sockfd;
    int u_epollfd;
    int u_duration_ms;
    const int u_len_t;
    //fec
    int u_data_shards;
    int u_parity_shards;
    FEC u_fec;
    //msg
    std::deque<std::pair<byte*, size_t>> u_msg_deque;
    std::mutex u_lock;
    std::condition_variable u_cv;
    std::mutex u_queueLock;
    std::deque<std::string> u_recv_qeque;
    std::condition_variable u_rcv;
    std::mutex u_recv_lock;
    std::deque<std::string> u_recover_deque;
    int u_recv_count;
    int u_recover_count;
    //threads
    std::thread u_msg_thread;
public:
    //calculate time
    static uint32_t start_ts;
    static uint32_t end_ts;
};


#endif //UDP_SERVER_H

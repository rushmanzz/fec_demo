

#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H
#include <string>
#include <netinet/in.h>
#include <thread>
#include <chrono>
#include <fec.h>

class UDPClient {
public:
    UDPClient(std::string &ip, int port, int data_shards, int parity_shards);

    ~UDPClient();

    int set_no_block();

    void connect();

    void send(std::string& data) ;

    void set_fec(int &data_shards, int &parity_shards);

private:
    std::string u_ip;
    int u_port;
    int u_sockfd;
    struct sockaddr_in u_addr;
    //fec
    int u_data_shards;
    int u_parity_shards;
    FEC u_fec;
    std::vector<row_type> u_shards;
    uint32_t u_pkt_idx;
    //data
    byte u_encode_buf[1024];
    uint32_t u_send_count;
    uint32_t u_send_parity_count;
};


#endif //UDP_CLIENT_H

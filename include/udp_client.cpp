
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
#include "encoding.h"
#include "udp_client.h"

UDPClient::UDPClient(std::string &ip, int port, int data_shards, int parity_shards) :
    u_ip(ip), u_port(port), u_send_count(0), u_send_parity_count(0), u_pkt_idx(0) {
    if (data_shards > 0 && parity_shards > 0 && parity_shards <= data_shards) {
        set_fec(data_shards, parity_shards);
    } else {
        u_data_shards = 0;
        u_parity_shards = 0;
    }
}

UDPClient::~UDPClient() {
    ::close(u_sockfd);
}

int UDPClient::set_no_block() {
    int sock_flag = ::fcntl(u_sockfd, F_GETFL);
    if (sock_flag < 0) {
        return sock_flag;
    }
    sock_flag |= O_NONBLOCK;
    ::fcntl(u_sockfd, F_SETFL, sock_flag);
    return 0;
}

void UDPClient::connect() {
    u_sockfd = ::socket(PF_INET, SOCK_DGRAM, 0);
    assert(u_sockfd != -1);
    int ret = set_no_block();
    assert(ret != -1);
    //addr
    //struct sockaddr_in addr;
    {
        ::bzero(&u_addr, sizeof(u_addr));
        u_addr.sin_family = PF_INET;
        u_addr.sin_port = htons(u_port);
        assert(::inet_pton(PF_INET, u_ip.c_str(), &u_addr.sin_addr) != -1);
    }
    ret = ::connect(u_sockfd, (struct sockaddr*)&u_addr, sizeof(u_addr));
    assert(ret != -1);
    {
        ::bzero(&u_addr, sizeof(u_addr));
        u_addr.sin_family = PF_INET;
        u_addr.sin_port = htons(u_port);
        assert(::inet_pton(PF_INET, u_ip.c_str(), &u_addr.sin_addr) != -1);
    }
}

void UDPClient::send(std::string& data) {
    if (u_data_shards == 0) {
        int ret = ::sendto(u_sockfd, data.c_str(), data.length(), 0, (struct sockaddr*)&u_addr, sizeof(u_addr));
        std::cout << "send ret: " << ret << std::endl;
        if (ret > 0) {
            std::cout << "send count: " << ++u_send_count << std::endl;
        }
        return;
    }
    int len = data.length();
    //u_encode_buf = new byte[1500];
    ::memcpy(u_encode_buf+fecHeaderSizePlus2, data.c_str(), static_cast<size_t>(len));
    u_fec.MarkData(u_encode_buf, static_cast<uint16_t>(len));
    int ret = ::sendto(u_sockfd, u_encode_buf, len+fecHeaderSizePlus2, 0, (struct sockaddr*)&u_addr, sizeof(u_addr));
    if (ret > 0) {
        //std::cout << "send count: " << ++u_send_count <<std::endl;
        u_shards[u_pkt_idx] = std::make_shared<std::vector<byte>>(&u_encode_buf[fecHeaderSize], &u_encode_buf[len+fecHeaderSizePlus2]);
        u_pkt_idx++;
        if (u_pkt_idx == u_data_shards)
        {
            u_fec.Encode(u_shards);
            for(size_t i = u_data_shards; i < u_data_shards + u_parity_shards; i++)
            {
                ::memcpy(u_encode_buf+fecHeaderSize, u_shards[i]->data(), u_shards[i]->size());
                u_fec.MarkFEC(u_encode_buf);
                //didn't do this------------
                ::sendto(u_sockfd, u_encode_buf, u_shards[i]->size()+fecHeaderSize, 0, (struct sockaddr*)&u_addr, sizeof(u_addr));
                //std::cout << "typeFEC: " << std::string((char*)u_encode_buf, fecHeaderSize, u_shards[i]->size()+fecHeaderSize) << std::endl;
                std::cout << "time: " << ++u_send_parity_count << std::endl;
            }
            u_pkt_idx = 0;
        }
    }
    //delete [] u_encode_buf;
    //u_encode_buf = nullptr;
}

void UDPClient::set_fec(int &data_shards, int &parity_shards) {
    u_data_shards = data_shards;
    u_parity_shards = parity_shards;
    u_fec = FEC::New(3 * (u_data_shards + u_parity_shards), u_data_shards, u_parity_shards);
    u_shards.resize(u_data_shards + u_parity_shards, nullptr);
}
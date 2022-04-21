

#include <iostream>
#include <memory>
#include <config.h>
#include <udp_server.h>

int main() {
    std::shared_ptr<Config> config_instance = Config::get_instance();
    std::string path("../configs/server_1_1.json");
    config_instance->load(path);
    UDPServer udpServer(config_instance->get_ip(),
                        config_instance->get_port(),
                        config_instance->get_data_shards(),
                        config_instance->get_parity_shards(),
                        config_instance->get_duration());
    std::cout<<"create a server successfull!"<<std::endl;
    udpServer.run();
}
/*
#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string>
#include <chrono>
#include <arpa/inet.h>
#include <thread>
#include <cstring>
#include <memory>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <fec.h>
#include "config.h"

extern bool Kill;

int set_no_block(int &sockfd) {
    int sock_flag = ::fcntl(sockfd, F_GETFL);
    sock_flag |= O_NONBLOCK;
    return ::fcntl(sockfd, F_SETFL, sock_flag);
}

void signal_interrupt(int signal_num) {
    Kill = true;
    std::cout << "signal_num: " << signal_num << " interrupt." << std::endl;
}

int main() {
    signal(SIGINT, signal_interrupt);
    std::shared_ptr<Config> config_instance = Config::get_instance();
    std::string path("../configs/server_1_1.json");
    config_instance->load(path);
    const std::string ip = config_instance->get_ip();
    const int port = config_instance->get_port();
    const int duration_ms = config_instance->get_duration();
    const int LEN_T = 1500;
    const int data_shards = config_instance->get_data_shards();
    const int parity_shards = config_instance->get_parity_shards();

    int sockfd = ::socket(PF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);
    struct sockaddr_in addr;
    {
        ::bzero(&addr, sizeof(addr));
        addr.sin_family = PF_INET;
        addr.sin_port = htons(port);
        assert(::inet_pton(PF_INET, ip.c_str(), &addr.sin_addr) != -1);
    }
    int ret = ::bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    assert(ret != -1);

    //epoll
    int epollfd = ::epoll_create(1);
    assert(epollfd != -1);
    struct epoll_event ev;
    ::bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    ::epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);

    struct epoll_event listen_ev[1];
    int wait_nums = 0;
    byte buf[LEN_T];
    int len = 0;
    while(!Kill) {
        wait_nums = ::epoll_wait(epollfd, listen_ev, 1, duration_ms);
        if (wait_nums < 0) {
            std::cout << "wait_nums < 0" << std::endl;
            exit(wait_nums);
        }
        for (int i = 0; i < wait_nums; i++) {
            if (listen_ev[i].data.fd == sockfd) {
                len = ::recv(sockfd, buf, LEN_T, 0);
                if (len < 0) {
                    std::cout << "read err_code_" << len << std::endl;
                    continue;
                }
                //std::cout << std::string(buf, buf+len) << std::endl;
            }
        }
    }

    return 0;
}
 */

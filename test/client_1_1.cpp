
#include <iostream>
#include <memory>
#include <config.h>
#include <udp_client.h>

int main() {
    std::cout<<"create a client!"<<std::endl;
    std::shared_ptr<Config> config_instance = Config::get_instance();
    std::string path("../configs/client_1_1.json");
    if (config_instance->load(path) == -1) {
        std::cout << "load path err: " << path << std::endl;
        return 0;
    }
    //std::cout<<"config load sucssessfull!"<<std::endl;
    UDPClient udpClient(config_instance->get_ip(),
                        config_instance->get_port(),
                        config_instance->get_data_shards(),
                        config_instance->get_parity_shards());
    udpClient.connect();
    //std::cout<<"connect successfull!"<<std::endl;
    std::string str = config_instance->get_data();
    int total_count = config_instance->get_send_count();
    int send_count = 0;
    const int duration_ms = config_instance->get_duration();
    while (send_count < total_count) {
        udpClient.send(str);
        send_count++;
        //std::cout<<"send a msg"<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    }
}
/*
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <assert.h>
#include <chrono>
#include <thread>
#include <cstring>
#include <config.h>
#include <fec.h>
#include <encoding.h>

bool _Kill = false;

int set_no_block(int &sockfd) {
    int sock_flag = ::fcntl(sockfd, F_GETFL);
    sock_flag |= O_NONBLOCK;
    return ::fcntl(sockfd, F_SETFL, &sock_flag);
}

int main() {
    //conf
    std::shared_ptr<Config> config_instance = Config::get_instance();
    std::string path = "../configs/client_1_1.json";
    config_instance->load(path);
    const int duration_ms = config_instance->get_duration();
    const std::string ip = config_instance->get_ip();
    const std::string data = config_instance->get_data();
    const int count = config_instance->get_send_count();
    const int data_shards = config_instance->get_data_shards();
    const int parity_shards = config_instance->get_parity_shards();
    //sock
    int sockfd = ::socket(PF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);
    struct sockaddr_in addr;
    ::bzero(&addr, sizeof(addr));
    addr.sin_family = PF_INET;
    addr.sin_port = htons(config_instance->get_port());
    assert(::inet_pton(PF_INET, ip.c_str(), &addr.sin_addr) != -1);
    assert(::connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) != -1);
    //send
    int sent = 0;
    while(sent < count && !_Kill) {
        assert(::send(sockfd, data.c_str(), data.length(), 0));
        sent++;
        std::cout << "send_count: " << sent << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    }

    return 0;
}
 */

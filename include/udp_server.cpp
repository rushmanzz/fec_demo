
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
#include "encoding.h"
#include "udp_server.h"

bool Kill = false;

uint32_t UDPServer::start_ts = 0;
uint32_t UDPServer::end_ts = 0;

void signal_handler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
    std::cout << "Time to receive the package: " << UDPServer::end_ts - UDPServer::start_ts << "ms" << std::endl;
    Kill = true;
    exit(signum);
}

UDPServer::UDPServer(std::string &ip, int port, int data_shards, int parity_shards, int duration_ms) :
    u_ip(ip),
    u_port(port),
    u_duration_ms(duration_ms),
    u_len_t(1500),
    u_recv_count(0),
    u_recover_count(0) {
    if (data_shards > 0 && parity_shards > 0 && parity_shards <= data_shards) {
        set_fec(data_shards, parity_shards);
    } else {
        u_data_shards = 0;
        u_parity_shards = 0;
    }
}

UDPServer::~UDPServer() {
    u_msg_thread.join();
    if (!u_msg_deque.empty()) {
        while(!u_msg_deque.empty()) {
            auto it = u_msg_deque.front();
            delete [] it.first;
            it.first = nullptr;
            u_msg_deque.pop_front();
        }
    }
    ::close(u_sockfd);
}

int UDPServer::set_no_block() {
    int sock_flag = ::fcntl(u_sockfd, F_GETFL);
    if (sock_flag < 0) {
        return sock_flag;
    }
    sock_flag |= O_NONBLOCK;
    ::fcntl(u_sockfd, F_SETFL, sock_flag);
    return 0;
}

void UDPServer::bind() {
    u_sockfd = ::socket(PF_INET, SOCK_DGRAM, 0);
    assert(u_sockfd != -1);
    //int ret = set_no_block();
    //assert(ret != -1);
    //addr
    struct sockaddr_in addr;
    {
        ::bzero(&addr, sizeof(addr));
        addr.sin_family = PF_INET;
        addr.sin_port = htons(u_port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    int ret = ::bind(u_sockfd, (struct sockaddr*)&addr, sizeof(addr));
    assert(ret != -1);
    //epoll
    /*
    u_epollfd = ::epoll_create(1);
    assert(u_epollfd != -1);
    struct epoll_event ev;
    ::bzero(&ev, sizeof(ev));
    ev.data.fd = u_sockfd;
    ev.events = EPOLLIN | EPOLLET;
    ::epoll_ctl(u_epollfd, EPOLL_CTL_ADD, u_sockfd, &ev);
     */
}

void UDPServer::run() {
    ::signal(SIGINT, signal_handler);
    bind();
    //u_msg_thread = std::thread(&UDPServer::msg_process, this);
    byte buf[u_len_t];
    int len = 0;
    struct sockaddr_in raddr;
    ::bzero(&raddr, sizeof(raddr));
    socklen_t raddr_len = 0;
    if (u_data_shards > 0) {
        //std::cout<<"listening KIll:"<<Kill<<std::endl;
        while(!Kill) {
            ::bzero(buf, u_len_t);
            //std::cout<<"in loop..."<<std::endl;
            //len = ::recvfrom(u_sockfd, buf, u_len_t, u_duration_ms, (struct sockaddr*)&raddr, &raddr_len);
            len = ::recv(u_sockfd, buf, u_len_t, u_duration_ms);
            //std::cout<<len<<std::endl;
            if (len < 0) {
                std::cout << "read err_code_" << len << std::endl;
                std::cout<<errno<<std::endl;
                continue;
            } else {
                std::pair<byte*, size_t> msg_map = std::make_pair<byte*, size_t>(buf, len);
                if(UDPServer::start_ts == 0){
                    start_ts = currentMs();
                }
                UDPServer::end_ts = currentMs();
                decode_fec(msg_map);
            }
        }
    } else {
        while(!Kill) {
            ::bzero(buf, u_len_t);
            len = ::recvfrom(u_sockfd, buf, u_len_t, u_duration_ms, (struct sockaddr*)&raddr, &raddr_len);
            if (len < 0) {
                //std::cout << "read err_code_" << len << std::endl;
                continue;
            } else {
                u_recv_count++;
                std::cout << "recv: " << buf << " recv_count: " << u_recv_count << std::endl;
            }
        }
    }
    
    /*
    struct epoll_event listen_ev[1];
    int wait_nums = 0;
    while(!Kill) {
        wait_nums = ::epoll_wait(u_epollfd, listen_ev, 1, u_duration_ms);

        if (wait_nums < 0) {
            std::cout << "wait_nums < 0" << std::endl;
            //exit(wait_nums);
        }
        for (int i = 0; i < wait_nums; i++) {
            if (listen_ev[i].data.fd == u_sockfd) {
                std::cout << "u_epollfd: " << u_epollfd << std::endl;
                ::bzero(buf, u_len_t);
                len = ::recv(u_sockfd, buf, u_len_t, 0);
                if (len < 0) {
                    std::cout << "read err_code_" << len << std::endl;
                    continue;
                }
                if (u_data_shards > 0) {
                    std::pair<byte*, size_t> msg_map = std::make_pair<byte*, size_t>(buf, len);
                    decode_fec(msg_map);
                } else {
                    u_recv_count++;
                    std::cout << "recv: " << buf << " recv_count: " << u_recv_count << std::endl;
                }*/
                /*
                byte* msg = new byte[len+1];
                ::memcpy(msg, buf, len);
                msg[len] = '\0';
                std::unique_lock<std::mutex> lock(u_lock);
                u_msg_deque.push_back(std::pair<byte*, size_t>(msg, len+1));
                //std::cout << std::string(buf, buf+len) << std::endl;
                 */
    /*        }
        }
    }*/
}

void UDPServer::set_fec(int data_shards, int parity_shards) {
    u_data_shards = data_shards;
    u_parity_shards = parity_shards;
    u_fec = FEC::New(3 * (u_data_shards + u_parity_shards), u_data_shards, u_parity_shards);
}

void UDPServer::msg_process() {
    while(!Kill) {
        std::unique_lock<std::mutex> lock(u_lock);
        while (!u_msg_deque.empty()) {
            std::pair<byte*, size_t> msg_map = u_msg_deque.front();
            u_msg_deque.pop_front();
            if (msg_map.first == nullptr) {
                continue;
            }
            if (u_data_shards > 0) {
                decode_fec(msg_map);
            } else {
                u_recv_count++;
                std::cout << "recv: " << msg_map.first << " recv_count: " << u_recv_count << std::endl;
            }
            {
                delete [] msg_map.first;
                msg_map.first = nullptr;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(u_duration_ms));
    }
}

void UDPServer::decode_fec(std::pair<byte*, size_t> &msg_map) {
    auto pkt= u_fec.Decode(msg_map.first, static_cast<size_t>(msg_map.second));
    std::cout << "packet type:" << (pkt.flag == ( uint16_t)0xf2 ? "fec" : "data")  << "    packet seq: " << pkt.seqid ;
    if (pkt.flag == typeData)
    {
        std::string str((char*)pkt.data->data()+2, pkt.data->size()-2);
        u_recv_count++;
        std::cout << "    recv: " << str ;
    }
    std::string str((char*)pkt.data->data()+2, pkt.data->size()-2);
    //std::cout << "recv: " << str << " recv_count: " << u_recv_count << std::endl;
    //std::cout << "pkt.seqid: " << pkt.seqid << " pkt.flag: " << pkt.flag << std::endl;
    
    if (pkt.flag == typeData || pkt.flag == typeFEC)
    {
        auto recovered = u_fec.Input(pkt);
        //std::cout << "recovered.size:" <<recovered.size() << std::endl;
        for (auto &r : recovered)
        {
            //std::cout << "r->size: " << r->size() << " ";
            if (r->size() > 2)
            {
                auto ptr = r->data();
                uint16_t sz;
                decode16u(ptr, &sz);
                if (sz >= 2 && sz <= r->size()) {
                    std::string str((char *) ptr + 2, sz - 2);
                    {
                        u_recv_count++;
                        u_recover_count++;
                    }
                    std::cout << "    recover: " << str ;
                }
            }
        }
    }
    std::cout << "  recv_count:" << u_recv_count << "  recover_count:" << u_recover_count << std::endl;
}

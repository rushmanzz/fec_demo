
#include <iostream>
#include <memory>
#include <config.h>
#include <tcp_client.h>

int main(){
    std::cout << "create a tcp client!" << std::endl;
    std::shared_ptr<Config> config_instance = Config::get_instance();
    std::string path("../configs/tcp_client_1.json");
    if(config_instance->load(path) == -1) {
        std::cout << "load path err: " << path << std::endl;
        return 0;
    }
    TCPClient tcpclient(config_instance->get_ip(),
                        config_instance->get_port());
    tcpclient.connect();
    std::string data = config_instance->get_data();
    int total_count = config_instance->get_send_count();
    int send_count = 0;
    const int duration_ms = config_instance->get_duration();
    while(send_count < total_count) {
        tcpclient.send(data);
        send_count++;
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    }
}


#include <iostream>
#include <memory>
#include <config.h>
#include <tcp_server.h>

int main() {
    std::shared_ptr<Config> config_instance= Config::get_instance();
    std::string path("../configs/tcp_server_1.json");
    config_instance->load(path);
    TCPServer tcpserver(config_instance->get_ip(),
                        config_instance->get_port(),
                        config_instance->get_duration());
    std::cout << "create a tcp server succeddfull!" << std::endl;
    tcpserver.run();
}
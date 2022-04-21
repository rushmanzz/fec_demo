#pragma once
#include <string>
#include <memory>

class Config
{
public:
    Config() : _data_shards(0), _parity_shards(0) { }
    ~Config() { }
    static std::shared_ptr<Config> get_instance()
    {
        if (_instance == nullptr)
        {
            _instance = std::make_shared<Config>();
        }
        return _instance;
    }
    int load(std::string &path);
    std::string& get_ip() { return _ip; }
    uint16_t& get_port() { return _port; }
    uint16_t& get_data_shards() { return _data_shards; }
    uint16_t& get_parity_shards() { return _parity_shards; }
    std::string& get_data() { return _data; }
    int& get_duration() { return _duration_millisecond; }
    int& get_send_count() { return _send_count; }
private:
    std::string _data;
    std::string _ip;
    uint16_t _port;
    uint16_t _data_shards;
    uint16_t _parity_shards;
    int _duration_millisecond;
    int _send_count;
    static std::shared_ptr<Config> _instance;
};

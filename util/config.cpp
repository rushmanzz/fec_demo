#include "config.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

const uint32_t FILE_SIZE = 65535;
std::shared_ptr<Config> Config::_instance = nullptr;

int Config::load(std::string &path)
{
    FILE* fileFd = ::fopen(path.c_str(), "rb");
    if (!fileFd)
    {
        std::cout << "open file error!" << std::endl;
        return -1;
    }
    //read
    char* jsonStr = new char[FILE_SIZE];
    int ret = ::fread(jsonStr, 1, FILE_SIZE, fileFd);
    ::fclose(fileFd);
    //parseJson
    rapidjson::Document d;
    if (d.Parse(jsonStr).HasParseError())
    {
        std::cout << "parse error!" << std::endl;
        return -1;
    }
    if (!d.IsObject())
    {
        std::cout << "not has object!" << std::endl;
        return -1;
    }
    if (d.HasMember("data") && d["data"].IsString())
    {
        _data = d["data"].GetString();
        std::cout << "data: " << _data << " len: " << _data.length() << std::endl;
    }
    if (d.HasMember("ip") && d["ip"].IsString())
    {
        _ip = d["ip"].GetString();
        std::cout << "ip: " << _ip << std::endl;
    }
    if (d.HasMember("port") && d["port"].IsInt())
    {
        _port = d["port"].GetInt();
        std::cout << "port: " << _port << std::endl;
    }
    if (d.HasMember("duration") && d["duration"].IsInt())
    {
        _duration_millisecond = d["duration"].GetInt();
        std::cout << "duration: " << _duration_millisecond << std::endl;
    }
    if (d.HasMember("data_shards") && d["data_shards"].IsInt())
    {
        _data_shards = d["data_shards"].GetInt();
        std::cout << "data_shards: " << _data_shards << std::endl;
    }
    if (d.HasMember("parity_shards") && d["parity_shards"].IsInt())
    {
        _parity_shards = d["parity_shards"].GetInt();
        std::cout << "parity_shards: " << _parity_shards << std::endl;
    }
    if (d.HasMember("count") && d["count"].IsInt())
    {
        _send_count = d["count"].GetInt();
        std::cout << "count: " << _send_count << std::endl;
    }
    {
        delete [] jsonStr;
        jsonStr = nullptr;
    }
    return 0;
}
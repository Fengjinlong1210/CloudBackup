#pragma once
#include <iostream>
#include <mutex>
#include "util.hpp"

namespace Cloud
{
// 单例类
#define CONFIG_FILE "cloud.conf"
    class Config
    {
    public:
        static Config *GetInstance()
        {
            // 双重判断, 第一次判断提高效率, 避免每次获取实例都需要加锁, 影响效率
            if (_instance == nullptr)
            {
                _mtx.lock();
                if (_instance == nullptr)
                {
                    _instance = new Config;
                }
                _mtx.unlock();
            }
            return _instance;
        }

        int GetHotTime()
        {
            return _hot_time;
        }
        int GetServerPort()
        {
            return _server_port;
        }
        std::string GetServerIP()
        {
            return _server_ip;
        }
        std::string GetDownloadPrefix()
        {
            return _download_prefix;
        }
        std::string GetPackfileSuffix()
        {
            return _packfile_suffix;
        }
        std::string GetPackDir()
        {
            return _pack_dir;
        }
        std::string GetBackDir()
        {
            return _back_dir;
        }
        std::string GetBackupFile()
        {
            return _backup_file;
        }

    private:
        Config()
        {
            ReadConfig();
        }
        bool ReadConfig()
        {
            // 对配置文件进行反序列化
            FileUtil fu(CONFIG_FILE);
            std::string configBody;
            if (!fu.GetContent(&configBody))
            {
                std::cout << "ReadConfig::Get config content failed" << std::endl;
                return false;
            }
            //std::cout << configBody << std::endl;

            Json::Value root;
            if (!JsonUtil::Deserialize(configBody, &root))
            {
                std::cout << "ReadConfig::Deserialize config content failed" << std::endl;
                return false;
            }
            _hot_time = root["hot_time"].asInt();
            _server_port = root["server_port"].asInt();
            _server_ip = root["server_ip"].asString();
            _download_prefix = root["download_prefix"].asString();
            _packfile_suffix = root["packfile_suffix"].asString();
            _pack_dir = root["pack_dir"].asString();
            _back_dir = root["back_dir"].asString();
            _backup_file = root["backup_file"].asString();
            return true;
        }

    private:
        int _hot_time;                // 热点控制时间
        int _server_port;             // 服务器端口
        std::string _server_ip;       // 服务器IP
        std::string _download_prefix; // 文件下载URL前缀
        std::string _packfile_suffix; // 压缩文件后缀
        std::string _pack_dir;        // 压缩文件路径
        std::string _back_dir;        // 上传备份文件路径
        std::string _backup_file;     // 服务器备份文件存放路径
        // 实例
        static Config *_instance;
        static std::mutex _mtx;
    };
    Config *Config::_instance = nullptr;
    std::mutex Config::_mtx;
}
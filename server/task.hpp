#pragma once
#include "util.hpp"

namespace Cloud
{
    class Task
    {
    public:
        Task(){}    //提供一个默认构造函数, 供线程池使用
        Task(FileUtil util, const std::string &pack_path)
            : _util(util), _pack_path(pack_path)
        {
        }

        bool operator()()
        {
            std::cout << "compress" << std::endl;
            _util.Compress(_pack_path);
            _util.Remove();
        }

    private:
        FileUtil _util;
        std::string _pack_path;
    };
}
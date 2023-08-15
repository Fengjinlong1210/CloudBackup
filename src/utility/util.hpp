#pragma once
#include <iostream>
#include <fstream>
#include <ostream>
#include <vector>
#include <string>
#include <ctime>
#include <memory>
#include <experimental/filesystem>
#include <jsoncpp/json/json.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "bundle.h"

namespace Cloud
{
    namespace fs = std::experimental::filesystem;
    class FileUtil
    {
    public:
        FileUtil(const std::string &filename)
            : _FileName(filename)
        {
        }
        // 获取文件大小
        int64_t FileSize()
        {
            // int stat(const char *path, struct stat *buf);
            struct stat st;
            int ret = stat(_FileName.c_str(), &st);
            if (ret < 0)
            {
                std::cout << "FileSize::get file size failed" << std::endl;
                return -1;
            }
            return st.st_size;
        }
        // 获取文件最后访问时间
        time_t LastAccessTime()
        {
            struct stat st;
            int ret = stat(_FileName.c_str(), &st);
            if (ret < 0)
            {
                std::cout << "LastAccessTime::get last access time failed" << std::endl;
                return -1;
            }
            return st.st_atime;
        }
        // 获取文件最后修改时间
        time_t LastModifyTime()
        {
            struct stat st;
            int ret = stat(_FileName.c_str(), &st);
            if (ret < 0)
            {
                std::cout << "LastModifyTime::get last modify time failed" << std::endl;
                return -1;
            }
            return st.st_mtime;
        }
        // 获取文件名
        std::string FileName()
        {
            auto pos = _FileName.rfind('/');
            if (pos == std::string::npos)
            {
                return _FileName;
            }
            return _FileName.substr(pos + 1);
        }
        // 获取指定位置开始制定长度的内容
        bool GetPosLen(std::string *body, size_t pos, size_t len)
        {
            // 判断大小
            size_t fsize = FileSize();
            if (pos + len > fsize)
            {
                std::cout << "GetPosLen::get content from pos failed" << std::endl;
                return false;
            }
            // 打开文件
            std::ifstream ifs;
            ifs.open(FileName().c_str(), std::ios::binary);
            if (!ifs.is_open())
            {
                std::cout << "GetPosLen::open file failed" << std::endl;
                return false;
            }
            // 进行读取
            ifs.seekg(pos, std::ios::beg); // 从begin开始偏移pos个位置
            body->resize(len);
            ifs.read(&(*body)[0], len);
            if (!ifs.good())
            {
                std::cout << "GetPosLen::read file error" << std::endl;
                ifs.close();
                return false;
            }
            ifs.close();
            return true;
        }
        // 获取文件内容
        bool GetContent(std::string *body)
        {
            return GetPosLen(body, 0, FileSize());
        }
        // 设置文件内容
        bool SetContent(const std::string &body)
        {
            std::ofstream ofs;
            // 打开文件
            ofs.open(FileName(), std::ios::binary);
            if (!ofs.is_open())
            {
                std::cout << "SetContent::open file failed" << std::endl;
                return false;
            }
            // 写入
            ofs.write(&body[0], body.size());
            if (!ofs.good())
            {
                std::cout << "SetContent::Set content error" << std::endl;
                ofs.close();
                return false;
            }
            ofs.close();
            return true;
        }
        bool Compress(const std::string &packedName)
        {
            // 1. 获取原文件内容
            std::string oriBody;
            if (!GetContent(&oriBody))
            {
                std::cout << "Compress::Get original content failed" << std::endl;
                return false;
            }
            // 2. 对原本内容进行压缩
            std::string packedBody = bundle::pack(bundle::LZIP, oriBody);
            // 3. 向新文件中写入
            FileUtil packedfile(packedName);
            if (!packedfile.SetContent(packedBody))
            {
                std::cout << "Compress::Set packed content failed" << std::endl;
                return false;
            }
            return true;
        }
        // 对文件进行解压缩
        bool Uncompress(const std::string &unpackedName)
        {
            // 1. 读取解压文件的内容
            std::string packedBody;
            if (!GetContent(&packedBody))
            {
                std::cout << "Uncompress::Get packed content failed" << std::endl;
                return false;
            }
            // 2. 对内容进行解压缩
            std::string oriBody;
            oriBody = bundle::unpack(packedBody);
            // 3. 将解压缩的内容重新放入新文件
            FileUtil unpackedfile(unpackedName);
            if (!unpackedfile.SetContent(oriBody))
            {
                std::cout << "Uncompress::Set original content falied" << std::endl;
                return false;
            }
            return true;
        }
        // 判断文件是否存在
        bool Exists()
        {
            // 判断路径是否存在
            return fs::exists(_FileName);
        }
        // 创建目录
        bool CreateDirectory()
        {
            // 给定路径, 创建目录
            //  如果路径已经存在
            if (Exists())
                return true;
            // 路径不存在时, 创建路径
            return fs::create_directory(_FileName);
        }
        // 遍历目录中的文件
        bool ScanDirectory(std::vector<std::string> *array)
        {
            // 获取当前路径下所有的目录和文件
            for (auto &p : fs::directory_iterator(_FileName))
            {
                //  如果是路径就跳过
                if (fs::is_directory(p))
                    continue;
                // 把文件内容放入vector
                array->push_back(fs::path(p).relative_path().string());
            }
            return true;
        }

    private:
        std::string _FileName;
    };

    class JsonUtil
    {
    public:
        static bool Serialize(const Json::Value& root, std::string* str)
        {
            Json::StreamWriterBuilder swb;
            std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
            std::stringstream ss;
            if(sw->write(root, &ss) != 0)
            {
                std::cout << "Json::Serialize error" << std::endl;
                return false;
            }
            *str = ss.str();
            return true;
        }

        static bool Deserialize(const std::string& str, Json::Value* root)
        {
            Json::CharReaderBuilder crb;
            std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
            std::string errStr;
            if(!cr->parse(str.c_str(), str.c_str() + str.size(), root, &errStr))
            {
                std::cout << "Json::Deserialize error" << std::endl;
                return false;
            }
            return true;
        }
    };
}
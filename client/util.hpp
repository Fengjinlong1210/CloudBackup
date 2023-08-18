#pragma once
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <iostream>
#include <cstdio>
#include <fstream>
#include <ostream>
#include <vector>
#include <string>
#include <ctime>
#include <memory>
#include <experimental/filesystem>
#include <sys/stat.h>

namespace Cloud
{
    namespace fs = std::experimental::filesystem;
    class FileUtil
    {
    public:
        FileUtil(const std::string& filename)
            : _filename(filename)
        {
        }
        // 获取文件大小
        size_t FileSize()
        {
            // int stat(const char *path, struct stat *buf);
            struct stat st;
            int ret = stat(_filename.c_str(), &st);
            if (ret < 0)
            {
                std::cout << "FileSize::get file size failed" << std::endl;
                return 0;
            }
            return st.st_size;
        }
        // 获取文件最后访问时间
        time_t LastAccessTime()
        {
            struct stat st;
            int ret = stat(_filename.c_str(), &st);
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
            int ret = stat(_filename.c_str(), &st);
            if (ret < 0)
            {
                std::cout << "LastModifyTime::get last modify time failed" << std::endl;
                return -1;
            }
            return st.st_mtime;
        }
        // 截取最后的文件名
        std::string FileName()
        {
            //auto pos = _filename.rfind('/');
            //if (pos == std::string::npos)
            //{
            //    return _filename;
            //}
            //return _filename.substr(pos + 1);
            return fs::path(_filename).filename().string();
        }
        // 获取指定位置开始制定长度的内容
        bool GetPosLen(std::string* body, size_t pos, size_t len)
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
            ifs.open(_filename, std::ios::binary);
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
        bool GetContent(std::string* body)
        {
            return GetPosLen(body, 0, FileSize());
        }
        // 设置文件内容
        bool SetContent(const std::string& body)
        {
            std::ofstream ofs;
            // 打开文件
            ofs.open(_filename, std::ios::binary);
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
        // 判断文件是否存在
        bool Exists()
        {
            // 判断路径是否存在
            return fs::exists(_filename);
        }
        // 创建目录
        bool CreateDirectory()
        {
            // 给定路径, 创建目录
            //  如果路径已经存在
            if (Exists())
                return true;
            // 路径不存在时, 创建路径
            return fs::create_directory(_filename);
        }
        // 遍历目录中的文件
        bool ScanDirectory(std::vector<std::string>* array)
        {
            // 获取当前路径下所有的目录和文件
            for (auto& p : fs::directory_iterator(_filename))
            {
                //  如果是路径就跳过
                if (fs::is_directory(p))
                    continue;
                // 把文件内容放入vector
                array->push_back(fs::path(p).relative_path().string());
            }
            return true;
        }

        bool Remove()
        {
            if (!Exists())
            {
                std::cout << "FileUtil::Remove failed: file not found" << std::endl;
                return false;
            }
            if (remove(_filename.c_str()) != 0)
            {
                std::cout << "FileUtil::Remove::remove failed" << std::endl;
                return false;
            }
            return true;
        }
        //private:
    public:
        std::string _filename;
    };

}
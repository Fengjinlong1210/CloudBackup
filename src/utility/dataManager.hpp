#pragma once
#include "util.hpp"
#include "config.hpp"

namespace Cloud
{
    // 该文件实现数据管理, 通过哈希表存储文件信息
    //  使用URL作为Key, 文件data作为Value

    struct BackupInfo
    {
        int _pack_flag;         // 标记为, 表示是否压缩
        time_t _atime;          // 最近一次访问时间
        time_t _mtime;          // 最近一次修改时间
        size_t _fsize;          // 文件大小
        std::string _real_path; // 实际存储路径
        std::string _pack_path; // 打包后路径
        std::string _url;       // 文件访问URL

        bool NewBackupInfo(const std::string &realpath)
        {
            // 对实际路径访问的文件做信息填充
            FileUtil fu(realpath);
            if(!fu.Exists())
            {
                std::cout << "NewBackupInfo:: file not exist";
                return false;
            }
            auto inst = Config::GetInstance();
            std::string downloadPrefix = inst->GetDownloadPrefix();  // 下载前缀
            std::string packfileSuffix = inst->GetPackfileSuffix();
            std::string packDir = inst->GetPackDir();
            _pack_flag = false;
            _atime = fu.LastAccessTime();
            _mtime = fu.LastModifyTime();
            _fsize = fu.FileSize();
            _real_path = realpath;
            // _pack_path = ./packDir + filename + suffix
            _pack_path = packDir + fu.FileName() + packfileSuffix;
            // _url = ./download/ + filename
            _url = downloadPrefix + fu.FileName();
        }
    };
}
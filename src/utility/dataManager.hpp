#pragma once
#include <unordered_map>
#include <pthread.h>
#include "util.hpp"
#include "config.hpp"

namespace Cloud
{
    // 该文件实现数据管理, 通过哈希表存储文件信息
    // 使用URL作为Key, 文件data作为Value

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
            if (!fu.Exists())
            {
                std::cout << "NewBackupInfo:: file not exist";
                return false;
            }
            auto inst = Config::GetInstance();
            std::string downloadPrefix = inst->GetDownloadPrefix(); // 下载前缀
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

    // 数据管理: 依赖读写锁、数据存储文件、hash
    class DateManager
    {
    public:
        DateManager()
        {
            // 获取备份文件名, 初始化锁, 加载文件内容
            _backup_file = Config::GetInstance()->GetBackupFile();
            pthread_rwlock_init(&_lock, nullptr);
            InitLoad();
        }

        bool InitLoad()
        {
            // 初始化服务器时, 从备份文件中读取内容
            // 1. 读取文件
            FileUtil fu(_backup_file);
            if(!fu.Exists())
            {
                std::cout << "InitLoad:: backup file not found" << std::endl;
                return false;
            }
            std::string body;
            fu.GetContent(&body);
            // 2. 反序列化
            Json::Value root;
            JsonUtil::Deserialize(body, &root);
            // 3. 填充table
            for(int i = 0; i < root.size(); ++i)
            {
                BackupInfo tmp;
                tmp._fsize = root[i]["_fsize"].asInt64();
                tmp._atime = root[i]["_atime"].asInt64();
                tmp._mtime = root[i]["_mtime"].asInt64();
                tmp._pack_flag = root[i]["_pack_flag"].asInt();
                tmp._pack_path = root[i]["_pack_path"].asString();
                tmp._real_path = root[i]["_real_path"].asString();
                tmp._url = root[i]["_url"].asString();
                Insert(tmp);
            }
            return true;
        }

        // 向备份文件写入当前_table的信息
        bool Storage()
        {
            // 1. 获取所有数据
            std::vector<BackupInfo> array;
            GetAllInfo(&array);
            // 2. 创建Json::Value, 填入信息
            Json::Value root;
            for(int i = 0; i < array.size(); ++i)
            {
                Json::Value item;
                item["_pack_flag"] = array[i]._pack_flag;
                item["_atime"] = (Json::Int64)array[i]._atime;
                item["_mtime"] = (Json::Int64)array[i]._mtime;
                item["_fsize"] = (Json::Int64)array[i]._fsize;
                item["_pack_path"] = array[i]._pack_path;
                item["_real_path"] = array[i]._real_path;
                item["_url"] = array[i]._url;
                root.append(item);
            }
            // 3. 序列化
            std::string body;
            JsonUtil::Serialize(root, &body);
            // 4. 写入文件
            FileUtil fu(_backup_file);
            fu.SetContent(body);
            return true;
        }

        bool Insert(const BackupInfo &val)
        {
            // 访_table这个临界资源时, 需要给写端上锁
            pthread_rwlock_wrlock(&_lock);
            _table[val._url] = val;
            pthread_rwlock_unlock(&_lock);
            // 插入数据后, 需要更新备份文件
            Storage();
            return true;
        }

        bool Update(const BackupInfo &val)
        {
            // 访_table这个临界资源时, 需要给写端上锁
            pthread_rwlock_wrlock(&_lock);
            _table[val._url] = val;
            pthread_rwlock_unlock(&_lock);
            // 插入数据后, 需要更新备份文件
            Storage();
            return true;
        }

        bool GetOneByURL(const std::string &url, BackupInfo *info)
        {
            // 根据url从文件中读取信息
            pthread_rwlock_wrlock(&_lock);
            auto it = _table.find(url);
            if (it == _table.end())
            {
                std::cout << "GetOneByURL:: Get BackupInfo by URL failed" << std::endl;
                pthread_rwlock_unlock(&_lock);
                return false;
            }
            *info = it->second;
            pthread_rwlock_unlock(&_lock);
            return true;
        }

        bool GetOneByRealPath(const std::string &realpath, BackupInfo *info)
        {
            // 遍历table
            pthread_rwlock_wrlock(&_lock);
            auto it = _table.begin();
            for (; it != _table.end(); ++it)
            {
                if (it->second._real_path == realpath)
                {
                    *info = it->second;
                    pthread_rwlock_unlock(&_lock);
                    return true;
                }
            }
            pthread_rwlock_unlock(&_lock);
            std::cout << "GetOneByURL:: Get BackupInfo by real path failed" << std::endl;
            return false;
        }

        bool GetAllInfo(std::vector<BackupInfo> *array)
        {
            pthread_rwlock_wrlock(&_lock);
            auto it = _table.begin();
            for (; it != _table.end(); ++it)
            {
                array->push_back(it->second);
            }
            pthread_rwlock_unlock(&_lock);
            return true;
        }


        bool Debug()
        {
            pthread_rwlock_wrlock(&_lock);
            auto it = _table.begin();
            for(; it != _table.end(); ++it)
            {
                std::cout << it->second._atime << std::endl;
                std::cout << it->second._mtime << std::endl;
                std::cout << it->second._fsize<< std::endl;
                std::cout << it->second._pack_flag << std::endl;
                std::cout << it->second._pack_path << std::endl;
                std::cout << it->second._real_path << std::endl;
                std::cout << it->second._url << std::endl;
                std::cout << "------------------------------" << std::endl;
            }
            pthread_rwlock_unlock(&_lock);
        }
    private:
        std::unordered_map<std::string, BackupInfo> _table;
        pthread_rwlock_t _lock;
        std::string _backup_file;
    };
}
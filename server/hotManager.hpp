#pragma once
#include <unistd.h>
#include "util.hpp"
#include "dataManager.hpp"
#include "config.hpp"
#include "task.hpp"
#include "threadPool.hpp"

// 热点文件管理模块, 热点文件: 在一定时间内访问过的文件
// 当一个文件超过一定时间没有访问时, 该文件不再是热点文件, 需要对该文件进行压缩
// 压缩后存储到特定的packdir

extern Cloud::DataManager *_dataMgr;

namespace Cloud
{
    class HotManager
    {
    public:
        HotManager()
        {
            // 初始化成员变量, 创建目录
            auto inst = Config::GetInstance();
            _pack_dir = inst->GetPackDir();
            _back_dir = inst->GetBackDir();
            _packfile_suffix = inst->GetPackfileSuffix();
            _hot_time = inst->GetHotTime();
            FileUtil backdir(_back_dir);
            FileUtil packdir(_pack_dir);
            backdir.CreateDirectory();
            packdir.CreateDirectory();
        }

        bool RunModule()
        {
            while (1)
            {
                // 1. 遍历备份目录, 获取文件
                FileUtil fu(_back_dir);
                std::vector<std::string> fileset;
                fu.ScanDirectory(&fileset);
                // for(auto& file : fileset)
                // {
                //     std::cout << "fileset: " << file << std::endl;
                // }
                // 2. 判断文件是不是热点文件
                for (auto &file : fileset)
                {
                    //std::cout << "file: " << file << std::endl;
                    // 热点文件不需要处理
                    if (IsHotFile(file))
                        continue;
                    // 3. 非热点文件, 获取备份信息
                    BackupInfo info;
                    // 如果该文件存在但是没有备份信息, 就创建一个备份信息
                    if (_dataMgr->GetOneByRealPath(file, &info) == false)
                    {
                        info.NewBackupInfo(file);
                    }
                    // 4. 需要对其进行压缩
                    FileUtil tmp(file);
                    Task task(tmp, info._pack_path);
                    ThreadPool<Task>::GetInstance()->PushTask(task);
                    //tmp.Compress(info._pack_path); // 备份信息中保存了该文件压缩后的路径
                    // 5. 删除原文件, 存放压缩文件
                    info._pack_flag = true;
                    _dataMgr->Update(info);
                }
                usleep(10000); // 防止空目录循环过快 消耗资源
            }
            return true;
        }

    private:
        bool IsHotFile(const std::string &filename)
        {
            // 如果是热点文件, true
            FileUtil fu(filename);
            time_t lat = fu.LastAccessTime();
            time_t cur = time(nullptr);
            if (cur - lat > _hot_time)
            {
                // 非热点文件
                return false;
            }
            return true;
        }

    private:
        std::string _pack_dir;        // 压缩文件路径
        std::string _back_dir;        // 上传备份文件路径
        std::string _packfile_suffix; // 压缩文件后缀
        int _hot_time;                // 热点控制时间
    };
}

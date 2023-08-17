#pragma once
#include <ctime>
#include <string>
#include "httplib.h"
#include "config.hpp"
#include "dataManager.hpp"
#include "util.hpp"
extern Cloud::DataManager *_dataMgr;
namespace Cloud
{
    class Service
    {
    public:
        Service()
        {
            auto inst = Config::GetInstance();
            _server_port = inst->GetServerPort();
            std::cout << _server_port << std::endl;

            _server_IP = inst->GetServerIP();
            std::cout << _server_IP << std::endl;

            _download_prifix = inst->GetDownloadPrefix();
            std::cout << "service init success" << std::endl;
        }

        bool RunModule()
        {
            // 注册回调函数
            _server.Post("/upload", Upload);
            _server.Get("/ListShow", ListShow);
            _server.Get("/", ListShow);
            std::string download_url = _download_prifix + "(.*)";
            _server.Get(download_url, Download);
            //_server.listen(_server_IP.c_str(), _server_port);
            // 由于我们是云服务器, 不需要监听ip
            _server.listen("0.0.0.0", _server_port);

            std::cout << "service running" << std::endl;
            return true;
        }

    private:
        // struct MultipartFormData
        // {
        //     std::string name;
        //     std::string content;
        //     std::string filename;
        //     std::string content_type;
        // };
        static void Upload(const httplib::Request &req, httplib::Response &resp)
        {
            //std::cout << "Upload" << std::endl;
            auto ret = req.has_file("file");
            if (!ret)
            {
                resp.status = 400;
                return;
            }

            // 从request中获取文件信息
            const auto &file = req.get_file_value("file");
            // 向backdir中创建文件并写入客户端传递过来的内容
            std::string back_dir = Config::GetInstance()->GetBackDir();
            std::string realpath = back_dir + FileUtil(file.filename).FileName();
            FileUtil fu(realpath);
            fu.SetContent(file.content);

            // 创建一个备份信息填入备份文件
            BackupInfo info;
            info.NewBackupInfo(realpath);
            _dataMgr->Insert(info);
        }

        static void ListShow(const httplib::Request &req, httplib::Response &resp)
        {
            // 1. 获取所有的文件备份信息
            std::vector<BackupInfo> arry;
            _dataMgr->GetAllInfo(&arry);
            // 2. 根据所有备份信息，组织html文件数据
            std::stringstream ss;
            ss << "<html><head><title>Download</title></head>";
            ss << "<body><h1>Download</h1><table>";
            for (auto &file : arry)
            {

                ss << "<tr>";
                std::string filename = FileUtil(file._real_path).FileName();
                ss << "<td><a href='" << file._url << "'>" << filename << "</a></td>";
                ss << "<td align='right'>" << TimetoStr(file._mtime) << "</td>";
                ss << "<td align='right'>" << file._fsize / 1024 << "k</td>";
                ss << "</tr>";
            }
            ss << "</table></body></html>";
            resp.body = ss.str();
            resp.set_header("Content-Type", "text/html");
            resp.status = 200;
            return;
        }

        static void Download(const httplib::Request &req, httplib::Response &resp)
        {
            // 1. 根据客户端请求的资源路径, 获取资源备份信息
            BackupInfo info;
            _dataMgr->GetOneByURL(req.path, &info);

            // 2. 根据备份文件信息判断文件是否被压缩, 如果被压缩, 需要进行解压缩
            if (info._pack_flag == true)
            {
                // 解压缩, 然后更改备份信息
                FileUtil fu(info._pack_path);
                fu.Uncompress(info._real_path);
                fu.Remove();
                info._pack_flag = false;
                _dataMgr->Update(info);
            }
            bool retrans = false;
            std::string old_ETag;
            if (req.has_header("If-Range"))
            {
                // 包含If-Range字段
                old_ETag = req.get_header_value("If-Range");
                if (old_ETag == GetEtag(info))
                {
                    // 两次请求的文件没有发生改变, 满足断点续传
                    retrans = true;
                }
            }

            // 3. 将解压缩后的内容写入响应正文body
            FileUtil fu(info._real_path);
            if (!retrans) // 不需要断点续传
            {
                fu.GetContent(&resp.body);
                // 4. 填充响应信息
                resp.set_header("Accept-Ranges", "bytes");
                resp.set_header("ETag", GetEtag(info));
                resp.set_header("Content-Type", "application/octet-stream");
                resp.status = 200;
            }
            else
            {
                // httplib实现了断点续传
                // 只需要把文件内容填充到响应报文中, 库会自动根据请求的区间
                // 帮我们划分出需要发送的内容
                fu.GetContent(&resp.body);
                // 4. 填充响应信息
                resp.set_header("Accept-Ranges", "bytes");
                resp.set_header("ETag", GetEtag(info));
                resp.set_header("Content-Type", "application/octet-stream");
                //resp.set_header("Content-Range", "bytes start-end/filesize");
                resp.status = 206;
            }
        }

        static std::string GetEtag(const BackupInfo &info)
        {
            // Etag = filename + filesize + mtime
            std::string tag;
            FileUtil fu(info._real_path);
            tag += fu.FileName() + "-" + std::to_string(info._fsize) + "-" + std::to_string(info._mtime);
            return tag;
        }

        static std::string TimetoStr(time_t t)
        {
            std::string ret = ctime(&t);
            return ret;
        }

    private:
        int _server_port;
        std::string _server_IP;
        std::string _download_prifix;
        httplib::Server _server;
    };
}
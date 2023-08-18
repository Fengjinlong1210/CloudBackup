#pragma once
#include <ctime>
#include "util.hpp"
#include "dataManager.hpp"
#include "httplib.h"

namespace Cloud
{
#define SERVER_HOST "43.138.29.200"
#define SERVER_PORT 8888
	class Backup
	{
	public:
		Backup(const std::string& backdir, const std::string& backup_file)
		{
			//初始化成员变量
			_back_dir = backdir;
			_data_mgr = new DataManager(backup_file);
		}

		void RunModule()
		{
			//循环读取备份文件目录，判断是否需要上传，将需要上传的进行上传
			while (1)
			{
				// 1. 遍历备份文件夹，获取所有备份文件
				FileUtil fu(_back_dir);
				std::vector<std::string> files;
				fu.ScanDirectory(&files);
				// 2. 逐个判读是否需要上传
				for (auto& file : files)
				{
					if (!IsNeedUpload(file))
					{
						continue;
					}

					if (Upload(file))
					{
						//上传成功，添加文件备份信息
						_data_mgr->Insert(file, GetFileId(file));
						std::cout << file << " upload success" << std::endl;
					}
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
				std::cout << "loop once" << std::endl;
			}
		}

		std::string GetFileId(const std::string& filename)
		{
			//文件id = 文件路径 + 文件大小 + 文件mtime
			std::string ret;
			FileUtil fu(filename);
			ret += fu.FileName() + "-";
			ret += std::to_string(fu.FileSize()) + "-";
			ret += std::to_string(fu.LastModifyTime());
			return ret;
		}

		bool Upload(const std::string& filename)
		{
			// 1. 获取文件数据
			FileUtil fu(filename);
			std::string body;
			fu.GetContent(&body);
			// 2. 创建客户端连接
			httplib::Client client(SERVER_HOST, SERVER_PORT);
			// 3. 添加报头， 发送
			httplib::MultipartFormData item;
			item.content = body;
			item.content_type = "application/octet-stream";
			item.filename = fu.FileName();
			item.name = "file";
			httplib::MultipartFormDataItems items;
			items.push_back(item);
			// 4. 发送数据
			auto ret = client.Post("/upload", items);
			if (!ret || ret->status != 200)
			{
				std::cout << "Upload:: client upload error" << std::endl;
				return false;
			}
			std::cout << "Upload:: client upload success" << std::endl;
			return true;
		}

		bool IsNeedUpload(const std::string& filename)
		{
			// 判断文件是否需要上传：遍历备份文件信息
			//1. 是不是新文件 2. 有没有修改过
			std::string ID;
			if (_data_mgr->GetOneByKey(filename, &ID) != false)
			{
				// 有该文件的备份信息，判断是否需要重新上传
				std::string new_ID = GetFileId(filename);
				if(ID == new_ID)
				{
					// 文件没有被修改
					return false;
				}
			}

			//如果一个大文件正在上传，则每次都需要重新上传
			// 所以要判断文件是否在一定时间内没有被修改过，才需要上传
			FileUtil fu(filename);
			if (time(nullptr) - fu.LastModifyTime() < 3)
			{
				return false;
			}
			std::cout << filename << " need upload" << std::endl;
			return true;
		}


	private:
		//备份路径
		std::string _back_dir;
		DataManager* _data_mgr;
	};
}
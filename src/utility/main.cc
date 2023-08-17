#include <iostream>
#include <thread>
#include "util.hpp"
#include "config.hpp"
#include "dataManager.hpp"
#include "hotManager.hpp"
#include "service.hpp"
// #include "bundle.h"
using namespace Cloud;
using namespace std;

// int main()
// {
//     //FileUtil fu("../utility/test.txt");
//     // FileUtil fu("test.txt");
//     // auto size = fu.FileSize();
//     // auto atime = fu.LastAccessTime();
//     // auto mtime = fu.LastModifyTime();
//     // auto name = fu.FileName();
//     // cout << size << endl;
//     // cout << atime << endl;
//     // cout << mtime << endl;
//     // cout << name << endl;

//     // FileUtil fu("../utility/test.txt");
//     // string content;
//     // fu.GetPosLen(&content, 0, 100);
//     // cout << content << endl;

//     // FileUtil fu("../utility/test.txt");
//     // //向文件写入
//     // string buffer;
//     // getline(cin, buffer);
//     // fu.SetContent(buffer);

//     // string content;
//     // fu.GetContent(&content);
//     // cout << content << endl;

//     // FileUtil fu("../utility/bundle.h");
//     // fu.Compress("b.h.lz");

//     // FileUtil ufu("../utility/b.h.lz");
//     // ufu.Uncompress("test.txt");

//     FileUtil fu("./hello");
//     cout << fu.Exists() << endl;
//     cout << fu.CreateDirectory() << endl;

//     vector<string> arr;
//     FileUtil fu2("./hello");
//     cout << fu2.Exists() << endl;
//     cout << fu2.ScanDirectory(&arr) << endl;
//     for(auto& str : arr)
//     {
//         cout << str << " ";
//     }
//     cout << endl;
//     return 0;
// }

// int main()
// {
//     Json::Value root;
//     float score[3] = {100, 69.5, 87.5};
//     root["姓名"] = "张三";
//     root["年龄"] = 18;
//     root["成绩"].append(score[0]);
//     root["成绩"].append(score[1]);
//     root["成绩"].append(score[2]);

//     string str;
//     JsonUtil::Serialize(root, &str);
//     cout << str << endl;

//     Json::Value val;
//     JsonUtil::Deserialize(str, &val);
//     cout << root["姓名"] << endl;
//     cout << root["年龄"] << endl;
//     for(int i = 0; i < 3; ++i)
//     {
//         cout << root["成绩"][i] << endl;
//     }
//     return 0;
// }

// int main()
// {
//     auto inst = Cloud::Config::GetInstance();
//     cout << inst->GetBackDir() << endl;
//     cout << inst->GetDownloadPrefix() << endl;
//     cout << inst->GetHotTime() << endl;
//     cout << inst->GetPackDir() << endl;
//     cout << inst->GetPackfileSuffix() << endl;
//     cout << inst->GetBackupFile() << endl;
//     cout << inst->GetServerIP() << endl;
//     cout << inst->GetServerPort() << endl;
//     return 0;
// }

// int main(int argc, char* args[])
// {
//     if(argc != 2) return -1;
//     string filename = args[1];
//     BackupInfo info1;
//     info1.NewBackupInfo(filename);
//     cout << info1._fsize << endl;
//     cout << info1._atime << endl;
//     cout << info1._mtime << endl;
//     cout << info1._pack_flag << endl;
//     cout << info1._pack_path<< endl;
//     cout << info1._real_path << endl;
//     cout << info1._url << endl;
//     return 0;
// }

// int main()
// {
//     FileUtil fu("./backdir/main");
//     fu.Compress("./packdir/main.lz");
// }

Cloud::DataManager *_dataMgr;

void DataManage()
{
    Cloud::HotManager hotmgr;
    hotmgr.RunModule();
}

void ServiceManage()
{
    Cloud::Service svc;
    svc.RunModule();
}

int main()
{
    _dataMgr = new DataManager();
    std::thread svc(ServiceManage);
    std::thread hot(DataManage);
    svc.join();
    hot.join();
    return 0;
}
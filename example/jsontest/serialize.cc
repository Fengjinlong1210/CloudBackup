#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <jsoncpp/json/json.h>
using namespace std;

int main()
{
    Json::Value root;
    float score[3] = {100, 69.5, 87.5};
    root["姓名"] = "张三";
    root["年龄"] = 18;
    root["成绩"].append(score[0]);
    root["成绩"].append(score[1]);
    root["成绩"].append(score[2]);

    Json::StreamWriterBuilder swb;  // 借助该对象创建StreamWriter, 用来序列化
    unique_ptr<Json::StreamWriter> writer(swb.newStreamWriter());
    ostringstream ostr;
    writer->write(root, &ostr);
    cout << ostr.str() << endl;
}
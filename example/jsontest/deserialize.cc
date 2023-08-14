#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <jsoncpp/json/json.h>
using namespace std;

// C++11支持的R"()" 原生字符串(Raw String)
// 使得字符串可以直接在内部使用换行符或者特殊字符, 不需要转义

int main()
{
    string str = R"({"姓名":"peter", "年龄":20, "成绩":[100, 98.5, 99.4]})";
    string errStr;
    Json::Value root;
    Json::CharReaderBuilder crb;                          // 使用该对象创建CharReader
    unique_ptr<Json::CharReader> cr(crb.newCharReader()); // CharReader用来进行字符串分割, 反序列化
    bool ret = cr->parse(str.c_str(), str.c_str() + str.size(), &root, &errStr);
    if (!ret)
    {
        cerr << "parse error" << endl;
        return -1;
    }
    cout << root["姓名"].asCString() << endl;
    cout << root["年龄"].asInt() << endl;
    int sz = root["成绩"].size();
    for(int i = 0; i < sz; ++i)
    {
        cout << root["成绩"][i] << endl;
    }
    return 0;
}
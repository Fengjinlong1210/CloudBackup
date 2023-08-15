#include <iostream>
#include "util.hpp"
//#include "bundle.h"
using namespace Cloud;
using namespace std;

int main()
{
    //FileUtil fu("../utility/test.txt");
    // FileUtil fu("test.txt");
    // auto size = fu.FileSize();
    // auto atime = fu.LastAccessTime();
    // auto mtime = fu.LastModifyTime();
    // auto name = fu.FileName();
    // cout << size << endl;
    // cout << atime << endl;
    // cout << mtime << endl;
    // cout << name << endl;

    // FileUtil fu("../utility/test.txt");
    // string content;
    // fu.GetPosLen(&content, 0, 100);
    // cout << content << endl;

    // FileUtil fu("../utility/test.txt");
    // //向文件写入
    // string buffer;
    // getline(cin, buffer);
    // fu.SetContent(buffer);

    // string content;
    // fu.GetContent(&content);
    // cout << content << endl;

    // FileUtil fu("../utility/bundle.h");
    // fu.Compress("b.h.lz");

    // FileUtil ufu("../utility/b.h.lz");
    // ufu.Uncompress("test.txt");

    FileUtil fu("./hello");
    cout << fu.Exists() << endl;
    cout << fu.CreateDirectory() << endl;

    vector<string> arr;
    FileUtil fu2("./hello");
    cout << fu2.Exists() << endl;
    cout << fu2.ScanDirectory(&arr) << endl;
    for(auto& str : arr)
    {
        cout << str << " ";
    }
    cout << endl;
    return 0;
}
#include <iostream>
#include <string>
#include <fstream>
#include "bundle.h"
using namespace std;

void Usage()
{
    cout << "Usage:" << endl;
    cout << "\t./main [args[1] = 原始文件] [args[2] = 目标文件]" << endl;
}

int main(int argc, char **args)
{
    if (argc != 3)
    {
        Usage();
        return -1;
    }
    // 获取原始文件和目标文件名
    string inFilename = args[1];
    string outFilename = args[2];

    //读取原始文件的内容到字符串中
    ifstream ifs;
    ifs.open(inFilename, ios::binary);//以二进制形式打开原始文件
    //seekg用来改变当前文件指针的指向
    ifs.seekg(0, ios::end); // 指向结尾
    //tellg获取当前的阅读位置, 因为把指针指向了末尾, 所以tellg会返回文件的长度
    size_t fsize = ifs.tellg();
    ifs.seekg(0, ios::beg);
    
    string fileBody;
    fileBody.resize(fsize);
    //将fsize的内容读取到字符串中
    ifs.read(&fileBody[0], fsize);
    
    //使用bundle提供的接口, 将压缩后的文件放入字符串
    string packedFile = bundle::pack(bundle::LZIP, fileBody);

    //创建一个目标文件, 向立面写入数据
    ofstream ofs;
    ofs.open(outFilename, ios::binary);
    ofs.write(&packedFile[0], packedFile.size());

    ifs.close();
    ofs.close();

    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include "bundle.h"
using namespace std;

void Usage()
{
    cout << "Usage:" << endl;
    cout << "\t./main [args[1] = 压缩文件] [args[2] = 解压缩文件]" << endl;
}

int main(int argc, char** args)
{
    if (argc != 3)
    {
        Usage();
        return -1;
    }

    string inFilename = args[1];
    string outFilename = args[2];

    ifstream ifs;
    ifs.open(inFilename, ios::binary);
    ifs.seekg(0, ios::end);
    size_t fsize = ifs.tellg();
    ifs.seekg(0, ios::beg);

    string fileBody;
    fileBody.resize(fsize);
    ifs.read(&fileBody[0], fsize);

    ofstream ofs;
    string unpackedFile = bundle::unpack(fileBody);
    ofs.open(outFilename, ios::binary);
    ofs.write(&unpackedFile[0], unpackedFile.size());

    ifs.close();
    ofs.close();

    return 0;
}
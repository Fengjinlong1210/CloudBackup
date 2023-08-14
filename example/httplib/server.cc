#include <iostream>
#include "httplib.h"

using namespace std;
using namespace httplib;

void GetHi(const Request& req, Response& resp)
{
    resp.set_content("hello, I am a server", "text/plain");
}

void GetNumber(const Request& req, Response& resp)
{
    auto path = req.matches[0];
    auto numbers = req.matches[1];
    string str = path.str() + numbers.str();
    resp.set_content(str, "text/plain");
}

void PostMultipart(const Request& req, Response& resp)
{
    auto ret = req.has_file("file");
    if(!ret)
    {
        cout << "not file upload" << endl;
        resp.status = 400;
        return;
    }
    const auto& file = req.get_file_value("file");
    resp.body.clear();
    resp.body += file.filename;
    resp.body += "\n";
    resp.body += file.content;
    resp.set_header("Content-Type", "text/plain");
    resp.status = 200;
    return;
}

int main()
{
    Server svr;
    //先注册对请求的回调函数
    svr.Get("/hi", GetHi);
    svr.Get(R"(/numbers/(\d+))", GetNumber);
    svr.Post("/multipart", PostMultipart);
    svr.listen("0.0.0.0", 8888);
    return 0;
}
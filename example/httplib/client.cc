#include <iostream>
#include "httplib.h"

#define SERVER_IP "43.138.29.200"
#define SERVER_PORT 8888

using namespace std;
using namespace httplib;

int main()
{
    Client client(SERVER_IP, SERVER_PORT);
    auto res0 = client.Get("/hi"); // res的返回值类型是指针
    cout << "Get /hi : " << endl;
    cout << "\tstatus: " << res0->status << endl;
    cout << "\tbody: " << res0->body << endl;

    auto res1 = client.Get("/numbers/12345678");
    cout << "Get /numbers/12345678 : " << endl;
    cout << "\tstatus: " << res1->status << endl;
    cout << "\tbody: " << res1->body << endl;

    MultipartFormData item;
    item.name = "file";
    item.filename = "hello.txt";
    item.content = "hello world";
    item.content_type = "text/plain";
    MultipartFormDataItems items;
    items.push_back(item);

    auto res2 = client.Post("/multipart", items);
    cout << "Post /multipart : " << endl;
    cout << "\tstatus: " << res2->status << endl;
    cout << "\tbody: " << res2->body << endl;

    return 0;
}
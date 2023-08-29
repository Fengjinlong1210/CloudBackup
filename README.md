云备份

服务端功能:
    1. 文件操作: 使用C++17的filesystem进行目录的创建和扫描, 获取文件操作时间、文件大小等信息, 对文件进行读写
    2. 配置文件读取: 将服务器端信息保存到配置文件, 设计一个单例类从配置文件中读取配置信息, 如需要对服务器相关信息进行修改, 只需修改配置文件, 同时用到了Json对信息进行序列化和反序列化
    3. 数据管理模块: 将备份的文件信息进行提取, 写入到指定的备份信息文件中, 使用unordered_map管理文件信息, 将文件的URL作为key值, 文件信息类作为value
    4. 热点管理模块: 对于一定时间没有访问的文件, 服务器自动对其进行压缩, 节约磁盘空间
    5. 多线程压缩: 引入线程池处理文件压缩任务, 避免过大的文件导致阻塞
    6. 服务器模块: 搭建HTTP服务器, 注册对HTTP请求的回调函数, 根据请求调用不同的处理函数
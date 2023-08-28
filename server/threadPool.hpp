#pragma once
#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <pthread.h>

namespace Cloud
{
    template <class T>
    class ThreadPool
    {
        static const size_t default_thread_cnt = 5;

    public:
        static ThreadPool<T> *GetInstance()
        {
            // if (_instance == nullptr)
            // {
            //     std::lock_guard<std::mutex> lock(_static_mutex);
            //     if (_instance == nullptr)
            //     {
            //         _instance = new ThreadPool<T>();
            //         _instance->init();
            //         std::cout << "get instance success" << std::endl;
            //     }
            // }
            // return _instance;
            static ThreadPool<T> _instance;
            return &_instance;
        }

    private:
        ThreadPool(size_t thread_cnt = default_thread_cnt)
            : _thread_cnt(thread_cnt)
        {
            init();
        }

        void init()
        {
            for (int i = 0; i < _thread_cnt; ++i)
            {
                // 给每个线程初始化
                _threads.push_back(std::thread(Thread_handler, this));
            }
        }

    public:
        void PushTask(const T &task)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            // 向任务队列推送任务
            _tasks.push(task);
            // 唤醒线程执行任务
            _cond.notify_one();
        }

        T PopTask()
        {
            T task = _tasks.front();
            _tasks.pop();
            return task;
        }

        bool isEmpty()
        {
            return _tasks.empty();
        }

        static void Thread_handler(void *args)
        {
            // 获取当前类的this指针
            ThreadPool<T> *ptr = static_cast<ThreadPool<T> *>(args);
            while (1)
            {
                T task;
                {
                    std::unique_lock<std::mutex> lock(ptr->_mutex);
                    ptr->_cond.wait(lock, [ptr]
                                    { return ptr->stop || !(ptr->_tasks.empty()); });
                    if (ptr->stop && ptr->_tasks.empty())
                        return;
                    // 跳出循环时, 已经存在任务
                    task = ptr->PopTask();
                }
                task();
            }
        }

        ~ThreadPool()
        {
            {
                std::unique_lock<std::mutex> lock(_mutex);
                stop = true;
            }
            _cond.notify_all();
            for (int i = 0; i < _thread_cnt; ++i)
            {
                _threads[i].join();
            }
        }

    private:
        bool stop = false;
        std::vector<std::thread> _threads; // 存储多个线程
        std::queue<T> _tasks;              // 存储线程处理任务
        size_t _thread_cnt;                // 线程数量
        std::mutex _mutex;
        // static std::mutex _static_mutex;   // 用来保证单例的实例化没有线程安全问题
        std::condition_variable _cond; // 环境变量, 控制线程按一定顺序访问临界资源
        // static ThreadPool<T> *_instance;
    };
    // template <class T>
    // std::mutex ThreadPool<T>::_static_mutex;
    // template <class T>
    // ThreadPool<T> *ThreadPool<T>::_instance = nullptr;
}
#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <stdio.h>
#include <ctime>
#include "block_queue.h"
#include "../lock/locker.h"
using namespace std;
class log
{
public:
    // 懒汉式单例模式,返回该实例

    static log *get_log()
    {
        static log instance;
        return &instance;
    }
    // 创建线程异步写日志
    static void *flush_log_thread(void *args)
    {
        log::get_log()->async_write_log();
    }
    bool init(int max_line = 5000, int m_longbuf_size = 10000, int max_quesize = 0);
    void write_log(int level, const char *format, ...);
    void flush(void);

private:
    log();
    virtual ~log();
    void async_write_log()
    {
        string task;
        while (task_que->pop(task))
        { // 异步写入线程
            mutex.lock();
            fputs(task.c_str(), m_fp);
            mutex.unlock();
        }
    }

    int is_asyn; // 异步模式
    int m_lonbuf_size;
    int max_line;
    long long m_count;
    char filename[256];
    FILE *m_fp;
    int today;
    char *log_buf;
    locker mutex;
    block_queue<string> *task_que;
};
#define LOG_DEBUG(format, ...)                               \
    {                                                        \
        log::get_log()->write_log(0, format, ##__VA_ARGS__); \
        log::get_log()->flush();                             \
    }
#define LOG_INFO(format, ...)                                \
    {                                                        \
        log::get_log()->write_log(1, format, ##__VA_ARGS__); \
        log::get_log()->flush();                             \
    }
#define LOG_WARN(format, ...)                                \
    {                                                        \
        log::get_log()->write_log(2, format, ##__VA_ARGS__); \
        log::get_log()->flush();                             \
    }
#define LOG_ERROR(format, ...)                               \
    {                                                        \
        log::get_log()->write_log(3, format, ##__VA_ARGS__); \
        log::get_log()->flush();                             \
    }

#endif // DEBUG

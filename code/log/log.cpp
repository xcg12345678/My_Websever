#include <iostream>
#include <stdio.h>
#include <cstdarg>
#include <pthread.h>
#include <ctime>
#include <cstring>
#include "log.h"
using namespace std;
log::log()
{
    m_count = 0;
    is_asyn = false;
}

log::~log()
{
    if (m_fp != NULL)
    {
        fclose(m_fp);
    }
}
bool log::init(int max_line, int m_longbuf_size, int max_quesize)
{
    if (max_quesize > 0)
    { // 异步写日志
        is_asyn = true;
        task_que = new block_queue<string>(max_quesize);
        pthread_t tid;
        pthread_create(&tid, NULL, flush_log_thread, NULL);
    }
    this->max_line = max_line;
    this->m_lonbuf_size = m_longbuf_size;
    // 文件的打开和文件名的定义
    std::time_t current_time_t = std::time(nullptr);
    // 转换为本地时间
    std::tm *local_time = std::localtime(&current_time_t);
    snprintf(filename, 255, "%d_%02d_%02dSeverlog", local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday);
    today = local_time->tm_mday;
    m_fp = fopen(filename, "a");
    if (m_fp == NULL)
    {
        return false;
    }
    return true;
}
void log::write_log(int level, const char *format, ...)
{
    char s[16];
    switch (level)
    {
    case 0:
        strcpy(s, "[debug]:");
        break;
    case 1:
        strcpy(s, "[info]:");
        break;
    case 2:
        strcpy(s, "[warn]:");
        break;
    case 3:
        strcpy(s, "[erro]:");
        break;
    default:
        strcpy(s, "[info]:");
        break;
    }
    // 判断是否需要新打开文件并写入
    mutex.lock();
    m_count++;
    std::time_t current_time_t = std::time(nullptr);
    std::tm *local_time = std::localtime(&current_time_t);
    if (today != local_time->tm_mday || m_count % max_line == 0)
    {
        flush();
        fclose(m_fp);
        if (today != local_time->tm_mday)
        {
            snprintf(filename, 255, "%d_%02d_%02dSeverlog", local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday);
            today = local_time->tm_mday;
            m_count = 0;
        }
        else
        {
            snprintf(filename, 255, "%d_%02d_%02dSeverlog%lld", local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday, m_count / max_line);
        }
        m_fp = fopen(filename, "a");
    }
    mutex.unlock();
    va_list valst;
    va_start(valst, format);
    mutex.lock();
    // 写入到buf中
    string log_str;
    int n = snprintf(log_buf, 42, "%d-%02d-%02d %02d:%02d:%02d %s ",
                     local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday,
                     local_time->tm_hour, local_time->tm_min, local_time->tm_sec, s);
    int m = vsnprintf(log_buf, m_lonbuf_size - n - 1, format, valst);
    log_buf[m + n] = '\n';
    log_buf[m + n + 1] = '\0';
    log_str = log_buf;
    mutex.unlock();
    if (is_asyn)
    {
        task_que->push(log_str);
    }
    else
    {
        mutex.lock();
        fputs(log_str.c_str(), m_fp);
        mutex.unlock();
    }
    va_end(valst);
}
void log::flush(void)
{
    mutex.lock();
    fflush(m_fp);
    mutex.unlock();
}
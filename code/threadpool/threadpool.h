#ifndef THREADPOLL_H
#define THREADPOLL_H

#include <iostream>
#include <pthread.h>
#include <list>
#include "../lock/locker.h"
#include "../mysql/my_sql.h"
#include "../log/log.h"
using namespace std;
template <typename T>
class thread_poll
{
public:
    thread_poll(int aact_mode, conn_sql *con_poll, int thread_num = 8);
    ~thread_poll();
    bool append(T *request);
    bool append(T *request, int state);

private:
    static void *worker(void *arg);
    void run();

private:
    int thread_num;
    list<T *> requests;
    locker mutex;
    sem m_queuestate;
    pthread_t *m_threads;
    conn_sql *con_poll;
    int act_mode;
};
template <class T>
thread_poll<T>::thread_poll(int aact_mode, conn_sql *con_poll, int thread_num) : act_mode(act_mode), con_poll(con_poll), thread_num(thread_num)
{
    if (thread_num < 0)
    {
        throw std::exception();
    }
    m_threads = new pthread_t[thread_num];
    for (int i = 0; i < thread_num; i++)
    {
        if (pthread_create(m_threads + i, NULL, worker, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
        if (pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}
template <class T>
thread_poll<T>::~thread_poll()
{
    delete[] m_threads;
}
template <class T>
bool thread_poll<T>::append(T *request)
{
    mutex.lock();
    requests.push_back(request);
    mutex.unlock();
    m_queuestate.post();
    return true;
}
template <class T>
bool thread_poll<T>::append(T *request, int state)
{
    mutex.lock();
    request->m_state = state;
    requests.push_back(request);
    mutex.unlock();
    m_queuestate.post();
    return true;
}
template <class T>
void *thread_poll<T>::worker(void *arg)
{
    thread_poll *thread_p = (thread_poll *)arg;
    thread_p->run();
    return pool;
}
template <class T>
void thread_poll<T>::run()
{
    while (true)
    {
        m_queuestate.wait();
        mutex.lock();
        if (requests.empty())
        {
            mutex.unlock;
            continue;
        }
        T *request = requests.front();
        requests.pop_front();
        mutex.unlock();
        if (!request)
        {
            continue;
        }
        if (act_mode == 1) // reactor模式
        {
        }
        else
        {
        }
    }
}
#endif // !1

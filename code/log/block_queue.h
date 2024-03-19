#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include "../lock/locker.h"
#include <iostream>
using namespace std;
template <typename T>
class block_queue
{
public:
    block_queue(int maxnum = 10000)
    {
        maxsize = maxnum;
        array = new T[maxnum];
        front = 0;
        tail = 0;
        m_size = 0;
    }
    ~block_queue()
    {
        delete[] array;
    }
    bool push(T &item)
    {
        mutex.lock();
        if (m_size >= maxsize)
        {
            m_cond.broadcast();
            mutex.unlock();
            return false;
        }
        array[tail] = item;
        tail = (tail + 1) % maxsize;
        m_size++;
        m_cond.broadcast();
        return true;
    }
    bool pop(T &item)
    {
        mutex.lock();
        while (m_size <= 0) //?为什么要while
        {
            if (!m_cond.wait(mutex.get()))
            {
                mutex.unlock();
                return false;
            }
        }
        item = array[front];
        front = (front + 1) % maxsize;
        m_size--;
        mutex.unlock();
        return true;
    }

private:
    int m_size;
    int maxsize;
    int front;
    int tail;
    T *array;
    locker mutex;
    cond m_cond;
};

#endif // DEBUG
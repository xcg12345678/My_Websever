#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <iostream>
#include "../lock/locker.h"
using namespace std;
template <typename T>
class block_queue
{
public:
    block_queue()
    {
    }
    ~block_queue()
    {
    }
    init()
    {
    }
    bool push(T &item)
    {
    }
    bool pop(T &item)
    {
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
#endif //

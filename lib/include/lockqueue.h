//异步的日志写队列

#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
using namespace std;

template<typename T>
class LockQueue
{
public:
    //多个工作线程会进行写入队列
    void push(const T& data)
    {
        //上锁，出生命周期自动释放锁
        lock_guard<mutex> lock(m_mutex);
        m_queue.push(data);
        //向队列添加数据后，队列不为空唤醒等待的线程，使用条件变量进行线程间通信
        m_condvariable.notify_one();
    }
    //只有一个专门的写线程会读出队列数据，随后写入文件
    T pop()
    {
        unique_lock<mutex> lock(m_mutex);
        while(m_queue.empty())
        {
            //日志队列为空,进入等待状态同时释放锁
            m_condvariable.wait(lock);

        }
        //被唤醒后，拿到锁，作弹出操作
        T data=m_queue.front();
        m_queue.pop();
        return data;
    }
private:
    queue<string> m_queue;
    mutex m_mutex;
    condition_variable m_condvariable;
}; 
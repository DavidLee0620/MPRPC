#pragma once
#define THREADED
#include <zookeeper/zookeeper.h>
#include <semaphore.h>
#include <string>
class zkClient
{
public:
    zkClient();
    ~zkClient();
    //zkclient启动连接zkserver,获得SESSION会话
    void start();
    //在zkserver上根据制定的path创建znode节点
    void Create(const char* path,const char* data,int datalen,int state=0);
    //根据znode节点路径获取znode节点的值
    std::string GetData(const char* path);

    
private:
    //zk客户端句柄
    zhandle_t *m_zhandle;
};
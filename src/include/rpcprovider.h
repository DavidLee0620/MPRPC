#pragma once
#include "google/protobuf/service.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
using namespace std;

//框架提供的专门发布rpc服务的网络对象类
class RpcProvider
{
public:
    //框架提供的可以发布rpc服务的函数接口，需要保持抽象性，不能写死特定的类
    void NotifyService(google::protobuf::Service *service);

    //启动RPC节点，开始提供远程网络调用服务
    void Run();
private:
    muduo::net::EventLoop m_eventLoop;
    void OnConnecction(const muduo::net::TcpConnectionPtr&);
    void OnMessage(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp);
};
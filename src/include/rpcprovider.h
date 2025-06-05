#pragma once
#include "google/protobuf/service.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <google/protobuf/descriptor.h>
#include <unordered_map>
#include <string>
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
    //service服务类型信息
    struct ServiceInfo
    {
        //保存service服务对象
        google::protobuf::Service *m_service;
        //保存method服务方法
        unordered_map<string,const google::protobuf::MethodDescriptor*> m_methodMap; 
    };
    //储存注册成功的所有服务和对应的服务方法的所有信息
    unordered_map<string,ServiceInfo> m_serviceMap;
    muduo::net::EventLoop m_eventLoop;
    void OnConnecction(const muduo::net::TcpConnectionPtr&);
    void OnMessage(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp);
};
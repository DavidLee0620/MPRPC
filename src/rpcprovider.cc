#include "rpcprovider.h"
#include "mprpcapplication.h"
#include <functional>
using namespace std;


//框架提供的可以发布rpc服务的函数接口，需要保持抽象性，不能写死特定的类
void RpcProvider::NotifyService(google::protobuf::Service *service)
{

}

//启动RPC节点，开始提供远程网络调用服务
void RpcProvider::Run()
{
    string ip=MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port=atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip,port);
    //创建TCPServer对象
    muduo::net::TcpServer server(&m_eventLoop,address,"RpcProvider");
    //绑定连接回调和消息读写回调函数
    server.setConnectionCallback(bind(&RpcProvider::OnConnecction,this,placeholders::_1));
    server.setMessageCallback(bind(&RpcProvider::OnMessage,this,placeholders::_1,placeholders::_2,placeholders::_3));
    //设置muduo库的线程数,1个为IO线程读取，剩下7个为工作线程，Reactor模型，；epoll+线程池
    server.setThreadNum(8);

    cout<<"RpcProvider start service at ip:"<<ip<<" port:"<<port<<endl;
    //启动网络服务
    server.start();
    //启动epollwait阻塞
    m_eventLoop.loop();
}

void RpcProvider::OnConnecction(const muduo::net::TcpConnectionPtr&)
{

}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp)
{

}
#include "rpcprovider.h"
#include "mprpcapplication.h"
#include <functional>

using namespace std;

/*
service_name ==> service_desc描述 ===> 多个method_name ===> 获取对应的method_desc描述
*/
//框架提供的可以发布rpc服务的函数接口，需要保持抽象性，不能写死特定的类
void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info;
    //获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc=service->GetDescriptor();
    string service_name=pserviceDesc->name();
    int methodCnt=pserviceDesc->method_count();
    cout<<"service name: "<<service_name<<endl;
    //获取服务对象指定下标的服务方法的描述 例如通过service->method->method_name->匹配到Login函数
    for(int i=0;i<methodCnt;i++)
    {
        const google::protobuf::MethodDescriptor* pmethodDesc=pserviceDesc->method(i);
        string method_name=pmethodDesc->name();
        cout<<"method name: "<<method_name<<endl;
        service_info.m_methodMap.insert({method_name,pmethodDesc}); 

    }
    service_info.m_service=service;
    m_serviceMap.insert({service_name,service_info});

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
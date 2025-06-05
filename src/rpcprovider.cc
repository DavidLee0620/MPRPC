#include "rpcprovider.h"
#include "mprpcapplication.h"
#include <functional>
#include "rpcheader.pb.h"
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

void RpcProvider::OnConnecction(const muduo::net::TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        //rpc client连接断开，关闭文件描述符fd
        conn->shutdown();
    }
}
/*
定义通讯的数据格式
框架内部Provider和Consumer要协商好通信的protobuf的数据类型
service_name method_name args 定义proto的message类型进行序列化和反序列化
header_size(4个字节)+header_str+args
header_str.size()+(service_name+method_name+args.size())+args防止粘包问题
string insert+copy
*/
//如果远程有一个rpc调用的服务 那么这个函数就会响应
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp)
{
    //接收远程调用请求的字符流  Login args
    string recv_buf=buffer->retrieveAllAsString();
    //拷贝4个字节的内容
    uint32_t header_size;
    recv_buf.copy((char *)&header_size,4,0);
    string rpc_header_str=recv_buf.substr(4,header_size);
    //从字节流或者字符流反序列化
    mprpc::RpcHeader rpcheader;
    string service_name;
    string method_name;
    uint32_t args_size;
    if(rpcheader.ParseFromString(rpc_header_str))
    {
        //反序列化成功
        service_name=rpcheader.service_name();
        method_name=rpcheader.method_name();
        args_size=rpcheader.args_size();
    }
    else
    {
        cout<<"rpc_header_str :"<<rpc_header_str<<"ParseFromString error!"<<endl;
        return;
    }
    string args_str=recv_buf.substr(4+header_size,args_size);
    cout<<"======================================="<<endl;
    cout<<"header_size:"<<header_size<<endl;
    cout<<"rpc_header_str:"<<rpc_header_str<<endl;
    cout<<"service_name:"<<service_name<<endl;
    cout<<"method_name:"<<method_name<<endl;
    cout<<"args_size:"<<args_size<<endl;
    cout<<"args_str:"<<args_str<<endl;
    cout<<"======================================="<<endl;
    //获取Method和Service对象
    auto it=m_serviceMap.find(service_name);
    if(it==m_serviceMap.end())
    {
        cout<<"service_name:"<<service_name<<" is not exist!"<<endl;
        return;
    }
    
    auto mit=it->second.m_methodMap.find(method_name);
    if(mit==it->second.m_methodMap.end())
    {
        cout<<"method_name:"<<method_name<<" is not exist!"<<endl;
        return;
    }
    google::protobuf::Service* service=it->second.m_service; //UserService class
    const google::protobuf::MethodDescriptor* method=mit->second;//Login()
    //生成获取rqc请求的request和respone
    google::protobuf::Message* request=service->GetRequestPrototype(method).New();
    google::protobuf::Message* response=service->GetResponsePrototype(method).New();
    if(!request->ParseFromString(args_str))
    {
        cout<<"request parse error,content:"<<args_str<<endl;
        return;
    }
    //给下面的method绑定一个Closure回调
    google::protobuf::Closure *done=google::protobuf::NewCallback<RpcProvider,const muduo::net::TcpConnectionPtr&,google::protobuf::Message*>(this,&RpcProvider::SendRpcResponse,conn,response);
    //根据远程调用的rpc请求，调用当前的rpc发布的方法
    //UserService.Login(controller,request,respones,done)
    service->CallMethod(method,nullptr,request,response,done);
}
//done.run()会回调这个绑定的函数
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message* response)
{
    //response序列化
    string response_str;
    if(response->SerializeToString(&response_str))
    {
        //序列化成功后，通过网络发送响应
        conn->send(response_str);
    }
    else
    {
        cout<<"Serialize response error!"<<endl;
    }
    conn->shutdown();//模拟http短连接服务，回复响应后断开连接，以便提供更多的连接

}
#include "mprpcchannel.h"
#include <string>
#include "rpcheader.pb.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include "mprpcapplication.h"
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;
/*
header_size+service_name method_name args_size+args_str
*/
//通过stub代理调用的rpc方法，都到这里进行序列化和网络发送
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done)
{

    const google::protobuf::ServiceDescriptor* sd=method->service();
    string service_name=sd->name();
    string method_name=method->name();
    string args_str;
    uint32_t args_size=0;
    //序列化参数args
    if(request->SerializeToString(&args_str))
    {
        args_size=args_str.size();
    }
    else
    {
        controller->SetFailed("Serialize request err!");
        return;
    }
    //获取定义数据格式中的数据
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);
    string rpc_header_str;
    uint32_t rpc_header_size;
    //序列化数据体
    if(rpcHeader.SerializeToString(&rpc_header_str))
    {
        rpc_header_size=rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("Serialize rpcHeader err!");
        return;
    }
    //组织带发送的rpc请求的数据字符串
    string send_rpc_str;
    send_rpc_str.insert(0,string((char*)&rpc_header_size,4));
    send_rpc_str+=rpc_header_str;
    send_rpc_str+=args_str;
    cout<<"======================================="<<endl;
    cout<<"header_size:"<<rpc_header_size<<endl;
    cout<<"rpc_header_str:"<<rpc_header_str<<endl;
    cout<<"service_name:"<<service_name<<endl;
    cout<<"method_name:"<<method_name<<endl;
    cout<<"args_size:"<<args_size<<endl;
    cout<<"args_str:"<<args_str<<endl;
    cout<<"======================================="<<endl;
    //发送tcp请求
    int clientfd=socket(AF_INET,SOCK_STREAM,0);
    if(clientfd==-1)
    {
        char buf[512]={0};
        sprintf(buf,"create client socket errno:%d",errno);
        controller->SetFailed(buf);
        return;
    }
    string ip=MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port=atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=inet_addr(ip.c_str());
    //连接服务
    if(connect(clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1)
    {
        close(clientfd);
        char buf[512]={0};
        sprintf(buf,"connect client socket errno:%d",errno);
        controller->SetFailed(buf);
        return;
    }
    //发送数据
    if(send(clientfd,send_rpc_str.c_str(),send_rpc_str.size(),0)==-1)
    {
        close(clientfd);
        char buf[512]={0};
        sprintf(buf,"send errno:%d",errno);
        controller->SetFailed(buf);
        return;
    }
    //接收响应
    char recv_buf[1024]={0};
    int recv_size=0;
    if((recv_size=recv(clientfd,recv_buf,1024,0))==-1)
    {
        close(clientfd);
        char buf[512]={0};
        sprintf(buf,"recv errno:%d",errno);
        controller->SetFailed(buf);
        return;
    }   
    //将响应反序列化
    //string response_str(buf,0,recv_size);//出现bug，遇到\0结束，导致反序列化失败

    //if(!(response->ParseFromString(response_str)))
    if(!response->ParseFromArray(recv_buf,recv_size))
    {
        close(clientfd);
        char buf[512]={0};
        sprintf(buf,"Parse response error! response_str:%s",recv_buf);
        controller->SetFailed(buf);
        return;
    }
    close(clientfd);
}
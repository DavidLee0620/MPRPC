#include "mprpcchannel.h"
#include <string>
#include "rpcheader.pb.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
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
        cout<<"Serialize request err!"<<endl;
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
        cout<<"Serialize rpcHeader err!"<<endl;
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
        cout<<"create client socket errno: "<<errno<<endl;
        exit(EXIT_FAILURE);
    }
    
}
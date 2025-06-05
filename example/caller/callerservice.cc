#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"
int main(int argc,char **argv)
{
    //要想使用rpc服务，就应该先进行rpc框架的初始化
    MprpcApplication::Init(argc,argv);

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    //rpc方法请求和参数
    fixbug::LoginRequest request;
    request.set_name("lee");
    request.set_pwd("123456");
    //rpc方法响应
    fixbug::LoginResponse response;

    stub.Login(nullptr,&request,&response,nullptr);//RpcChannel->RpcChannel::CallMethod 完成序列化和网络发送
    if(response.result().errcode()==0)
    {
        cout<<"rpc Login response: "<<response.success()<<endl;
    }
    else
    {
        cout<<"rpc Login errmsg: "<<response.result().errmsg()<<endl;
    }
    return 0;
}
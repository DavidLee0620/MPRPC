#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
using namespace std;

class UserService:public fixbug::UserServiceRpc{ //rpc发布端（生产者）rpc服务提供者 
public:
    bool Login(string name,string pwd)
    {
        cout<<"doing local service:Login"<<endl;
        cout<<"name:"<<name<<" pwd:"<<pwd<<endl;
        return true;
    }
    bool Register(string name,string pwd,uint32_t id)
    {
        cout<<"doing local service:Register"<<endl;
        cout<<"name:"<<name<<" pwd:"<<pwd<<"id"<<id<<endl;
        return true;
    }
    //重写基类UserServiceRpc中的虚函数
    // caller ===> Login(LoginRequest) ===> moduo ===> callee 
    // callee ===>  Login(LoginRequest) ===> 到下面重写的Login方法
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        //框架给业务上报请求参数LoginRequest，应用获取数据做本地业务
        string name=request->name();
        string pwd=request->pwd();
        //本地业务
        bool login_result=Login(name,pwd);
        //把响应写入包括错误码、错误信息、布尔值
        fixbug::ResultCode *code=response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);
        //回调执行 响应对象的序列化和网络发送
        done->Run();
    }
    void Register(google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done)
    {
        //框架给业务上报请求参数LoginRequest，应用获取数据做本地业务
        string name=request->name();
        string pwd=request->pwd();
        uint32_t id=request->id();
        //本地业务
        bool ret=Register(name,pwd,id);
        //把响应写入包括错误码、错误信息、布尔值
        fixbug::ResultCode *code=response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(ret);
        //回调执行 响应对象的序列化和网络发送
        done->Run();
    }
};

int main(int argc,char **argv)
{
    //初始化rpc框架
    MprpcApplication::Init(argc,argv);
    //provider是一个网络服务对象，把要发布的UserService对象发布到RPC节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());
    //运行服务，阻塞等待远程的rpc请求调用
    provider.Run();
}
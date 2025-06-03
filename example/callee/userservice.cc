#include <iostream>
#include "../user.pb.h"

using namespace std;

class UserService:public fixbug::UserServiceRpc{ //rpc发布端（生产者）rpc服务提供者 
public:
    bool Login(string name,string pwd)
    {
        cout<<"doing local service:Login"<<endl;
        cout<<"name:"<<name<<" pwd:"<<pwd<<endl;
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
};
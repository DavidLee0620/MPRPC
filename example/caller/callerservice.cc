#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "logger.h"
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
    //定义一个控制信息对象
    MprpcController controller;
    stub.Login(&controller,&request,&response,nullptr);//RpcChannel->RpcChannel::CallMethod 完成序列化和网络发送
    //通过controller控制信息判断序列化与网络发送和响应信息是否正确
    if(controller.Failed())
    {
        //cout<<"controller errText:"<<controller.ErrorText()<<endl;
        LOG_ERROR("controller errText:%s",controller.ErrorText().c_str());
    }
    else
    {
        if(response.result().errcode()==0)
        {
            //cout<<"rpc Login success: "<<response.success()<<endl;
            LOG_INFO("rpc Login success:%d",response.success());
        }
        else
        {
            //cout<<"rpc Login errmsg: "<<response.result().errmsg()<<endl;
            LOG_ERROR("rpc Login errmsg:%s",response.result().errmsg().c_str());
            
        }

        //调用Register方法
        fixbug::RegisterRequest req;
        req.set_id(2000);
        req.set_name("david");
        req.set_pwd("858698");
        fixbug::RegisterResponse rsp;
        stub.Register(nullptr,&req,&rsp,nullptr);
        if(rsp.result().errcode()==0)
        {
            //cout<<"rpc Register success: "<<rsp.success()<<endl;
            LOG_INFO("rpc Register success:%d",rsp.success());

        }
        else
        {
            //cout<<"rpc Register errmsg: "<<rsp.result().errmsg()<<endl;
            LOG_ERROR("rpc Register errmsg:%s",rsp.result().errmsg().c_str());
        }

    }
    


    return 0;
}
#pragma once
//框架的基础类,使用单例模式，全局共用一个基础类
class MprpcApplication
{
public:
    static void Init(int argc,char **argv);
    static MprpcApplication& GetInstance()
    {
        static MprpcApplication app;
        return app;
    }
private:
    MprpcApplication(){}
    MprpcApplication(const MprpcApplication&)=delete;
    MprpcApplication(MprpcApplication&&)=delete;
};
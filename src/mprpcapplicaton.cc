#include "mprpcapplication.h"
#include <unistd.h>
#include <iostream>
using namespace std;
MprpcConfig MprpcApplication::m_config;
void ShowArgsHelp()
{
    cout<<"format: command -i <configfile>"<<endl;
}

void MprpcApplication::Init(int argc,char **argv)
{
    if (argc<2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }
    //获取配置文件
    int c=0;
    string config_file;
    while((c=getopt(argc,argv,"i:"))!=-1)
    {
        switch (c)
        {
        case 'i':
            config_file=optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    //加载配置文件 rpcserver_ip rpcserver_port zookeeper_ip zookeeper_port
    m_config.LoadConfigFile(config_file.c_str());
    //cout<<"rpcserverip:"<<m_config.Load("rpcserverip")<<endl;
    //cout<<"rpcserverport:"<<m_config.Load("rpcserverport")<<endl;
    //cout<<"zookeeperip:"<<m_config.Load("zookeeperip")<<endl;
    //cout<<"zookeeperport:"<<m_config.Load("zookeeperport")<<endl;
}


MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}

MprpcConfig& MprpcApplication::GetConfig()
{
    return m_config;
}
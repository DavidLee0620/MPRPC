#include "zookeeperutil.h"
#include <iostream>
#include "mprpcapplication.h"
#include "logger.h"
zkClient::zkClient():m_zhandle(nullptr)
{

}
zkClient::~zkClient()
{
    if(m_zhandle!=nullptr)
    {
        zookeeper_close(m_zhandle);//关闭句柄释放资源
    }
}
//全局watcher观察器 zkserver给zkclient的通知
void global_watcher(zhandle_t* zh,int type,int state,const char* path,void* watcherCtx)
{
    if(type==ZOO_SESSION_EVENT)//回调的消息类型和会话相关
    {
        if(state==ZOO_CONNECTED_STATE)//server和client状态已连接
        {
            sem_t *sem=(sem_t*)zoo_get_context(zh);//获取信号量
            sem_post(sem);//信号量+1
        }
    }
}
/*
zookeeper_mt:多线程版本
API客户端提供了三个线程：
API调用线程：zookeeper_init
网络IO线程：connstr:host* 用的poll 客户端不需要高并发
watcher回调线程：

*/
//连接zkServer，获得会话
void zkClient::start()
{
    string host=MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    string port=MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    string connstr=host+":"+port;
    //开启内存,创建句柄
    m_zhandle=zookeeper_init(connstr.c_str(),global_watcher,30000,nullptr,nullptr,0);
    if(m_zhandle==nullptr)
    {
        LOG_ERROR("zookeeper_init error!");
        //cout<<"zookeeper_init error!"<<endl;
        exit(EXIT_FAILURE);
    }
    //创建信号量
    sem_t sem;
    sem_init(&sem,0,0);
    //绑定句柄信号量上下文
    zoo_set_context(m_zhandle,&sem);
    //阻塞等待信号量为1
    sem_wait(&sem);
    //cout<<"zookeeper_init success!"<<endl;
    LOG_INFO("zookeeper_init success!");
}

void zkClient::Create(const char* path,const char* data,int datalen,int state)
{
    char path_buffer[128];
    int buffer_len=sizeof(path_buffer);
    int flag;
    flag=zoo_exists(m_zhandle,path,0,nullptr);
    //先判断path路径下的节点是否存在，若存在则不再创建
    if(flag==ZNONODE)//不存在
    {
        //创建节点
        flag=zoo_create(m_zhandle,path,data,datalen,&ZOO_OPEN_ACL_UNSAFE,state,path_buffer,buffer_len);
        if(flag==ZOK)
        {
            //cout<<"znode create success...path: "<<path<<endl;
            LOG_INFO("znode create success...path: %s",path);

        }
        else
        {
            //cout<<"flag: "<<flag<<endl;
            //cout<<"znode create error...path: "<<path<<endl;
            LOG_ERROR("flag: %d",flag);
            LOG_ERROR("znode create error...path: %s",path);
            exit(EXIT_FAILURE);
        }
    }

}
//根据路径，获取对应节点的值
std::string zkClient::GetData(const char* path)
{
    char buffer[64];
    int bufferlen=sizeof(buffer);
    int flag=zoo_get(m_zhandle,path,0,buffer,&bufferlen,nullptr);
    if(flag!=ZOK)
    {
        //cout<<"get Znode error...path: "<<path<<endl;
        LOG_ERROR("get Znode error...path: %s",path);
        return "";
    }
    else
    {
        return buffer;
    }
}
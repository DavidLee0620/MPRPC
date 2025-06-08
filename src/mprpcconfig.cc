#include <string>
#include "mprpcconfig.h"
#include <iostream>
#include "logger.h"
using namespace std;


//加载解析文件
void MprpcConfig::LoadConfigFile(const char* config_file)
{
    FILE *pf=fopen(config_file,"r");
    if(pf==nullptr)
    {
        //cout<<config_file<<"is not exist!"<<endl;
        LOG_INFO("%s is not exist!",config_file);
        exit(EXIT_FAILURE);
    }
    //1.注释 2.正确配置项 3.开头多余的空格
    while(!feof(pf))
    {
        char buf[512]={0};
        fgets(buf,512,pf);
        //去掉前面的空格
        string src_buf(buf);
        Trim(src_buf);
        //注释
        if(src_buf[0]=='#'||src_buf.empty())
        {
            continue;
        }
        int idx=src_buf.find('=');
        if(idx==-1)
        {
            continue;
        }
        string key,value;
        key=src_buf.substr(0,idx);
        Trim(key);
        int endidx=src_buf.find('\n',idx);
        value=src_buf.substr(idx+1,endidx-idx-1);
        Trim(value);
        m_configMap.insert({key,value});
    }
}
//查询配置项信息
string MprpcConfig::Load(const string &key)
{
    auto it=m_configMap.find(key);
    if(it==m_configMap.end())
        return "";
    return it->second;
}


void MprpcConfig::Trim(string &src_buf)
{
    int idx=src_buf.find_first_not_of(' ');
    if(idx!=-1)
    {
        src_buf=src_buf.substr(idx,src_buf.size()-idx);
    }
    //去掉后面的空格
    idx=src_buf.find_last_not_of(' ');
    if(idx!=-1)
    {
        src_buf=src_buf.substr(0,idx+1);
    }
}
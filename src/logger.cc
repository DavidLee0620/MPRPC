#include "logger.h"
#include <time.h>
#include <iostream>
#include <stdio.h>
void Logger::SetLogLevel(LogLevel level)
{
    m_loglevel=level;
}
//把日志信息写入缓冲区lockqueue
void Logger::Log(string msg)
{
    m_lckQue.push(msg);
}
Logger& Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

Logger::Logger()
{
    //启动写日志线程
    thread writelogTask([&](){
        for(;;)
        {
            //获取日期，取队列中的信息，写入txt文件 追加
            time_t now=time(nullptr);
            tm* nowtm=localtime(&now);
            char file_name[128];
            sprintf(file_name,"%d-%d-%d-log.txt",nowtm->tm_year+1900,nowtm->tm_mon+1,nowtm->tm_mday);
            FILE* pf=fopen(file_name,"a+");
            if(pf==nullptr)
            {
                //cout<<file_name<<"fopen error!"<<endl;
                LOG_ERROR("fopen error!");
                exit(EXIT_FAILURE);
            }
            string msg=m_lckQue.pop();
            //加入时分秒
            char time_str[128]={0};
            sprintf(time_str,"%d:%d:%d =>[%s] ",nowtm->tm_hour,nowtm->tm_min,nowtm->tm_sec,(m_loglevel==INFO?"info":"error"));
            msg.insert(0,time_str);
            msg.append("\n");
            fputs(msg.c_str(),pf);
            fclose(pf);
        }
    });
    //设置分离线程，守护线程
    writelogTask.detach();
}
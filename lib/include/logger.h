#pragma once
#include "lockqueue.h"
enum LogLevel
{
    INFO,
    ERROR,
};


class Logger
{

public:
    //设置日志级别和信息
    void SetLogLevel(LogLevel level);
    void Log(string msg);
    static Logger& GetInstance();
private:
    int m_loglevel;
    LockQueue<string> m_lckQue;
    Logger();
    Logger(const Logger&)=delete;
    Logger(Logger&&)=delete;
};


#define LOG_INFO(logmsgformat,...)\
    do\
    {\
        Logger& logger=Logger::GetInstance();\
        logger.SetLogLevel(INFO);\
        char c[1024]={0};\
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);\
        logger.Log(c);\
    }while(0)

#define LOG_ERROR(logmsgformat,...)\
    do\
    {\
        Logger& logger=Logger::GetInstance();\
        logger.SetLogLevel(ERROR);\
        char c[1024]={0};\
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);\
        logger.Log(c);\
    }while(0)
#pragma once

#include <unordered_map>
#include <string>
using namespace std;
class MprpcConfig
{
public:
    //加载解析文件
    void LoadConfigFile(const char* config_file);
    //查询配置项信息
    string Load(const string &key);
    void Trim(string &src_buf);
private:
    unordered_map<string,string> m_configMap;
};
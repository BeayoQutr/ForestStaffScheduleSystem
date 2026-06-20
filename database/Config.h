#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

class Config
{
public:
    /// 从 database.ini 读取连接参数，若文件不存在则返回硬编码默认值
    static QString driver();
    static QString server();
    static int port();
    static QString databaseName();
    static QString user();
    static QString password();
};

#endif

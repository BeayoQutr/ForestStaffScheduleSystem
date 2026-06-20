#include "Config.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTextStream>

namespace {

QString configFilePath()
{
    // 优先查找可执行文件同目录下的 database.ini
    const QString exeDir = QCoreApplication::applicationDirPath();
    const QString iniPath = exeDir + "/database.ini";
    if (QFile::exists(iniPath)) {
        return iniPath;
    }
    // 开发阶段回退到项目源码目录
    const QString projectPath = exeDir + "/../../database.ini";
    if (QFile::exists(projectPath)) {
        return projectPath;
    }
    return QString();
}

QString readValue(const QString &key, const QString &defaultValue)
{
    const QString path = configFilePath();
    if (path.isEmpty()) {
        return defaultValue;
    }

    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("Database");
    const QString value = settings.value(key, defaultValue).toString();
    settings.endGroup();
    return value;
}

} // anonymous namespace

QString Config::driver()
{
    return readValue("Driver", "MySQL ODBC 9.7 Unicode Driver");
}

QString Config::server()
{
    return readValue("Server", "127.0.0.1");
}

int Config::port()
{
    const QString path = configFilePath();
    if (path.isEmpty()) {
        return 3306;
    }
    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("Database");
    const int value = settings.value("Port", 3306).toInt();
    settings.endGroup();
    return value;
}

QString Config::databaseName()
{
    return readValue("Database", "forest_schedule_system");
}

QString Config::user()
{
    return readValue("User", "root");
}

QString Config::password()
{
    return readValue("Password", "");
}

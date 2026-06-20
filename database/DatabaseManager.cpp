#include "DatabaseManager.h"
#include "Config.h"

#include <QMessageBox>
#include <QSqlError>
#include <QString>

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager manager;
    return manager;
}

bool DatabaseManager::connect()
{
    const QString connectionName = "forest_schedule_connection";

    if (QSqlDatabase::contains(connectionName)) {
        m_db = QSqlDatabase::database(connectionName);
    } else {
        // 使用 ODBC 方式连接 MySQL，避免 QMYSQL driver not loaded 问题
        m_db = QSqlDatabase::addDatabase("QODBC", connectionName);
    }

    QString connectionString =
        "DRIVER={" + Config::driver() + "};"
        "SERVER=" + Config::server() + ";"
        "PORT=" + QString::number(Config::port()) + ";"
        "DATABASE=" + Config::databaseName() + ";"
        "USER=" + Config::user() + ";"
        "PASSWORD=" + Config::password() + ";"
        "OPTION=3;";

    m_db.setDatabaseName(connectionString);

    if (!m_db.open()) {
        QMessageBox::critical(
            nullptr,
            "数据库连接失败",
            "无法通过 ODBC 连接 MySQL 数据库：\n" + m_db.lastError().text() +
            "\n\n请检查以下内容："
            "\n1. MySQL 服务是否已启动"
            "\n2. 数据库 " + Config::databaseName() + " 是否存在"
            "\n3. 用户名和密码是否正确"
            "\n4. ODBC 驱动名称是否正确（当前：" + Config::driver() + "）"
            "\n5. 是否安装的是 64 位 MySQL ODBC 驱动"
            "\n6. 可在可执行文件目录放置 database.ini 来自定义连接参数"
        );
        return false;
    }

    return true;
}

void DatabaseManager::close()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::isOpen() const
{
    return m_db.isOpen();
}

QSqlDatabase DatabaseManager::database() const
{
    return m_db;
}
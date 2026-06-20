#include "UserDAO.h"

#include "../database/DatabaseManager.h"

#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

QString UserDAO::hashPassword(const QString &password)
{
    return QString::fromLatin1(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

bool UserDAO::login(const QString &username, const QString &password)
{
    const QString hashedPassword = hashPassword(password);

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", hashedPassword);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "登录查询失败：\n" + query.lastError().text());
        return false;
    }

    return query.next() && query.value(0).toInt() > 0;
}

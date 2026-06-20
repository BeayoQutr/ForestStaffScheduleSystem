#ifndef USERDAO_H
#define USERDAO_H

#include <QString>

class UserDAO
{
public:
    bool login(const QString &username, const QString &password);
    static QString hashPassword(const QString &password);
};

#endif

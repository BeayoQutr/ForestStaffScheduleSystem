#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QDateTime>
#include <QString>

class Employee
{
public:
    int id = 0;
    QString employeeNo;
    QString name;
    QString gender;
    int age = 0;
    QString phone;
    QString position;
    QString department;
    bool active = true;
    QDateTime createdAt;
};

#endif

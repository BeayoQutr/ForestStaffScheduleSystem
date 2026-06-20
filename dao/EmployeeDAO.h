#ifndef EMPLOYEEDAO_H
#define EMPLOYEEDAO_H

#include "../model/Employee.h"

#include <QList>
#include <QString>

class QSqlQuery;

class EmployeeDAO
{
public:
    bool addEmployee(const Employee &employee);
    bool updateEmployee(const Employee &employee);
    bool deleteEmployee(int id);
    QList<Employee> getAllActiveEmployees();
    QList<Employee> searchByName(const QString &name);
    Employee getByEmployeeNo(const QString &employeeNo);

    Employee getById(int id);
    int countAllEmployees();
    int countActiveEmployees();

private:
    Employee mapEmployee(const QSqlQuery &query) const;
};

#endif

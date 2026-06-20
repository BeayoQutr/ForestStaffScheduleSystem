#ifndef EMPLOYEESERVICE_H
#define EMPLOYEESERVICE_H

#include "../dao/EmployeeDAO.h"

class EmployeeService
{
public:
    bool addEmployee(const Employee &employee);
    bool updateEmployee(const Employee &employee);
    bool deleteEmployee(int id);
    QList<Employee> getAllActiveEmployees();
    QList<Employee> searchByName(const QString &name);
    Employee getByEmployeeNo(const QString &employeeNo);

private:
    EmployeeDAO m_employeeDAO;
    bool validateEmployee(const Employee &employee);
};

#endif

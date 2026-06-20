#include "EmployeeDAO.h"

#include "../database/DatabaseManager.h"

#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

Employee EmployeeDAO::mapEmployee(const QSqlQuery &query) const
{
    const QSqlRecord record = query.record();
    Employee employee;
    employee.id = record.value("id").toInt();
    employee.employeeNo = record.value("employee_no").toString();
    employee.name = record.value("name").toString();
    employee.gender = record.value("gender").toString();
    employee.age = record.value("age").toInt();
    employee.phone = record.value("phone").toString();
    employee.position = record.value("position").toString();
    employee.department = record.value("department").toString();
    employee.active = record.value("active").toInt() == 1;
    employee.createdAt = record.value("created_at").toDateTime();
    return employee;
}

bool EmployeeDAO::addEmployee(const Employee &employee)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("INSERT INTO employees (employee_no, name, gender, age, phone, position, department, active) "
                  "VALUES (:employee_no, :name, :gender, :age, :phone, :position, :department, :active)");
    query.bindValue(":employee_no", employee.employeeNo);
    query.bindValue(":name", employee.name);
    query.bindValue(":gender", employee.gender);
    query.bindValue(":age", employee.age);
    query.bindValue(":phone", employee.phone);
    query.bindValue(":position", employee.position);
    query.bindValue(":department", employee.department);
    query.bindValue(":active", employee.active ? 1 : 0);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "添加员工失败：\n" + query.lastError().text());
        return false;
    }
    return true;
}

bool EmployeeDAO::updateEmployee(const Employee &employee)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE employees SET employee_no = :employee_no, name = :name, gender = :gender, "
                  "age = :age, phone = :phone, position = :position, department = :department "
                  "WHERE id = :id");
    query.bindValue(":employee_no", employee.employeeNo);
    query.bindValue(":name", employee.name);
    query.bindValue(":gender", employee.gender);
    query.bindValue(":age", employee.age);
    query.bindValue(":phone", employee.phone);
    query.bindValue(":position", employee.position);
    query.bindValue(":department", employee.department);
    query.bindValue(":id", employee.id);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "修改员工失败：\n" + query.lastError().text());
        return false;
    }
    return true;
}

bool EmployeeDAO::deleteEmployee(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE employees SET active = 0 WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "删除员工失败：\n" + query.lastError().text());
        return false;
    }
    return true;
}

QList<Employee> EmployeeDAO::getAllActiveEmployees()
{
    QList<Employee> employees;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM employees WHERE active = 1 ORDER BY id DESC");

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "查询员工失败：\n" + query.lastError().text());
        return employees;
    }

    while (query.next()) {
        employees.append(mapEmployee(query));
    }
    return employees;
}

QList<Employee> EmployeeDAO::searchByName(const QString &name)
{
    QList<Employee> employees;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM employees WHERE active = 1 AND name LIKE :name ORDER BY id DESC");
    query.bindValue(":name", "%" + name + "%");

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "按姓名查询失败：\n" + query.lastError().text());
        return employees;
    }

    while (query.next()) {
        employees.append(mapEmployee(query));
    }
    return employees;
}

Employee EmployeeDAO::getByEmployeeNo(const QString &employeeNo)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM employees WHERE employee_no = :employee_no LIMIT 1");
    query.bindValue(":employee_no", employeeNo);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "按员工编号查询失败：\n" + query.lastError().text());
        return Employee();
    }

    if (query.next()) {
        return mapEmployee(query);
    }
    return Employee();
}

Employee EmployeeDAO::getById(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT * FROM employees WHERE id = :id LIMIT 1");
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "按员工 ID 查询失败：\n" + query.lastError().text());
        return Employee();
    }

    if (query.next()) {
        return mapEmployee(query);
    }
    return Employee();
}

int EmployeeDAO::countAllEmployees()
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT COUNT(*) FROM employees");
    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "统计员工总人数失败：\n" + query.lastError().text());
        return 0;
    }
    return query.next() ? query.value(0).toInt() : 0;
}

int EmployeeDAO::countActiveEmployees()
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT COUNT(*) FROM employees WHERE active = 1");
    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "统计在职人数失败：\n" + query.lastError().text());
        return 0;
    }
    return query.next() ? query.value(0).toInt() : 0;
}

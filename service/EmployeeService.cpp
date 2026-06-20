#include "EmployeeService.h"

#include <QMessageBox>

bool EmployeeService::validateEmployee(const Employee &employee)
{
    if (employee.employeeNo.trimmed().isEmpty()) {
        QMessageBox::warning(nullptr, "输入错误", "员工编号不能为空。");
        return false;
    }
    if (employee.name.trimmed().isEmpty()) {
        QMessageBox::warning(nullptr, "输入错误", "员工姓名不能为空。");
        return false;
    }
    if (employee.age < 16 || employee.age > 80) {
        QMessageBox::warning(nullptr, "输入错误", "年龄应为 16 到 80 之间的合理数字。");
        return false;
    }
    return true;
}

bool EmployeeService::addEmployee(const Employee &employee)
{
    if (!validateEmployee(employee)) {
        return false;
    }

    Employee existing = m_employeeDAO.getByEmployeeNo(employee.employeeNo);
    if (existing.id != 0) {
        QMessageBox::warning(nullptr, "业务提示", "员工编号已存在，不能重复添加。");
        return false;
    }

    return m_employeeDAO.addEmployee(employee);
}

bool EmployeeService::updateEmployee(const Employee &employee)
{
    if (employee.id <= 0) {
        QMessageBox::warning(nullptr, "业务提示", "请先选择要修改的员工。");
        return false;
    }
    if (!validateEmployee(employee)) {
        return false;
    }

    Employee existing = m_employeeDAO.getByEmployeeNo(employee.employeeNo);
    if (existing.id != 0 && existing.id != employee.id) {
        QMessageBox::warning(nullptr, "业务提示", "员工编号已被其他员工使用。");
        return false;
    }

    return m_employeeDAO.updateEmployee(employee);
}

bool EmployeeService::deleteEmployee(int id)
{
    if (id <= 0) {
        QMessageBox::warning(nullptr, "业务提示", "请先选择要删除的员工。");
        return false;
    }
    return m_employeeDAO.deleteEmployee(id);
}

QList<Employee> EmployeeService::getAllActiveEmployees()
{
    return m_employeeDAO.getAllActiveEmployees();
}

QList<Employee> EmployeeService::searchByName(const QString &name)
{
    return m_employeeDAO.searchByName(name);
}

Employee EmployeeService::getByEmployeeNo(const QString &employeeNo)
{
    return m_employeeDAO.getByEmployeeNo(employeeNo);
}

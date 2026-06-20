#include "EmployeeComboHelper.h"

#include "../dao/EmployeeDAO.h"
#include "../model/Employee.h"

#include <QComboBox>
#include <QList>

void EmployeeComboHelper::loadEmployeesInto(QComboBox *combo, EmployeeDAO &dao)
{
    if (!combo) return;

    const int currentEmployeeId = combo->currentData().toInt();
    combo->clear();

    const QList<Employee> employees = dao.getAllActiveEmployees();
    for (const Employee &employee : employees) {
        combo->addItem(employee.employeeNo + " - " + employee.name, employee.id);
    }

    const int index = combo->findData(currentEmployeeId);
    if (index >= 0) {
        combo->setCurrentIndex(index);
    }
}

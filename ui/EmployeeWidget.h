#ifndef EMPLOYEEWIDGET_H
#define EMPLOYEEWIDGET_H

#include "../service/EmployeeService.h"

#include <QWidget>

class QComboBox;
class QLineEdit;
class QSpinBox;
class QTableWidget;

class EmployeeWidget : public QWidget
{
public:
    explicit EmployeeWidget(QWidget *parent = nullptr);

private:
    int m_currentId = 0;
    EmployeeService m_employeeService;
    QTableWidget *m_table = nullptr;
    QLineEdit *m_noEdit = nullptr;
    QLineEdit *m_nameEdit = nullptr;
    QComboBox *m_genderCombo = nullptr;
    QSpinBox *m_ageSpin = nullptr;
    QLineEdit *m_phoneEdit = nullptr;
    QLineEdit *m_positionEdit = nullptr;
    QLineEdit *m_departmentEdit = nullptr;

    Employee formEmployee() const;
    void loadAll();
    void fillTable(const QList<Employee> &employees);
    void clearForm();
    void onRowSelected(int row);
};

#endif

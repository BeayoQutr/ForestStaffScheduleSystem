#ifndef EMPLOYEECOMBOHELPER_H
#define EMPLOYEECOMBOHELPER_H

class QComboBox;
class EmployeeDAO;

/// 工具类：将在职员工列表加载到 QComboBox，并保留当前选中项
class EmployeeComboHelper
{
public:
    /// 从 EmployeeDAO 加载在职员工到 comboBox，格式为"编号 - 姓名"，data 存员工 id
    /// 加载前后会保留 comboBox 中当前选中的员工 id
    static void loadEmployeesInto(QComboBox *combo, EmployeeDAO &dao);
};

#endif
